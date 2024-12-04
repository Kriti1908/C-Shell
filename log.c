#include "headers.h"
#include "log.h"
#include "commands.h"
#include "pipe.h"
#include "system_commands.h"
#include "input_req.h"
#include "bg.h"
#include "ping.h"
#include "activities.h"
#include "hop.h"
#include "reveal.h"
#include "proclore.h"
#include "seek.h"


#define MAX_ENTRIES 15

void createLog(char* filepath, char* content) {
    int fd = open(filepath, O_RDWR | O_CREAT, 0644);
    if (fd == -1) {
        perror_red("open");
        return;
    }

    char *logs[15];
    int count = 0;
    char *fileContent = NULL;
    off_t fileSize = lseek(fd, 0, SEEK_END);

    if (fileSize > 0) {
        fileContent = malloc(fileSize + 1);
        if (fileContent == NULL) {
            perror_red("malloc");
            close(fd);
            return;
        }

        lseek(fd, 0, SEEK_SET);
        ssize_t bytesRead = read(fd, fileContent, fileSize);
        if (bytesRead == -1) {
            perror_red("read");
            free(fileContent);
            close(fd);
            return;
        }

        fileContent[bytesRead] = '\0';

        char *line = strtok(fileContent, "\n");
        while (line != NULL) {
            logs[count] = strdup(line);
            count++;
            line = strtok(NULL, "\n");
        }
    }

    if (count > 0 && strcmp(logs[count - 1], content) == 0) {       // Check if the last log is the same as the new log
        printf("The last log entry is the same as the new log entry. Not inserting.\n");
        for (int i = 0; i < count; i++) {
            free(logs[i]);
        }
        free(fileContent);
        close(fd);
        return;
    }

    if (count == 15) {              // If there are 15 logs, remove the first one
        free(logs[0]);
        for (int i = 1; i < 15; i++) {
            logs[i - 1] = logs[i];
        }
        count--;
    }

    logs[count] = strdup(content);      // Add new log
    count++;

    if (ftruncate(fd, 0) == -1) {       // empty file
        perror_red("ftruncate");
        close(fd);
        return;
    }

    off_t offset = 0;                   // Write logs back to the file
    for (int i = 0; i < count; i++) {
        size_t len = strlen(logs[i]);
        if (pwrite(fd, logs[i], len, offset) != len) {
            perror_red("write");
            close(fd);
            return;
        }
        offset += len;
        if (pwrite(fd, "\n", 1, offset) != 1) {
            perror_red("write");
            close(fd);
            return;
        }
        offset += 1;
        free(logs[i]);
    }

    free(fileContent);
    close(fd);
}

void printLogs(char* filepath) {
    int fd = open(filepath, O_RDONLY | O_CREAT, 0644);
    if (fd == -1) {
        perror_red("open");
        return;
    }

    char *logs[15];
    int count = 0;
    char *fileContent = NULL;
    off_t fileSize = lseek(fd, 0, SEEK_END);

    if (fileSize > 0) {
        fileContent = malloc(fileSize + 1);
        if (fileContent == NULL) {
            perror_red("malloc");
            close(fd);
            return;
        }

        lseek(fd, 0, SEEK_SET);
        ssize_t bytesRead = read(fd, fileContent, fileSize);
        if (bytesRead == -1) {
            perror_red("read");
            free(fileContent);
            close(fd);
            return;
        }

        fileContent[bytesRead] = '\0';

        char *line = strtok(fileContent, "\n");
        while (line != NULL) {
            logs[count] = strdup(line);
            count++;
            line = strtok(NULL, "\n");
        }
    }

    for (int i = 0; i < count; i++) {
        printf("%s\n", logs[i]);
        free(logs[i]);
    }

    free(fileContent);
    close(fd);

}

void purgeLogs(char* filepath) {
    int fd = open(filepath, O_RDWR | O_CREAT, 0644);
    if (fd == -1) {
        perror_red("open");
        return;
    }

    if (ftruncate(fd, 0) == -1) {
        perror_red("ftruncate");
        close(fd);
        return;
    }

    close(fd);
}

int executeLog(char* filepath, int logNum, char* lastdir, char* initialDirectory, int* sleepDuration, int* clockIndex, char** display_process_list, config_process* config_list, int configCommandsSize, char* logFile, int* isLogExit, char** pidOfBackgroundCommands, int* pidOfBackgroundCommandsIndex, bgProcess background_processes[]) {
    int fd = open(filepath, O_RDONLY | O_CREAT, 0644);
    if (fd == -1) {
        perror_red("open");
        return -1;
    }

    char *logs[15];
    int count = 0;
    char *fileContent = NULL;
    off_t fileSize = lseek(fd, 0, SEEK_END);
    int flag = 0;

    if (fileSize > 0) {
        fileContent = malloc(fileSize + 1);
        if (fileContent == NULL) {
            perror_red("malloc");
            close(fd);
            return -1;
        }

        lseek(fd, 0, SEEK_SET);
        ssize_t bytesRead = read(fd, fileContent, fileSize);
        if (bytesRead == -1) {
            perror_red("read");
            free(fileContent);
            close(fd);
            return -1;
        }

        fileContent[bytesRead] = '\0';

        char *line = strtok(fileContent, "\n");
        while (line != NULL) {
            logs[count] = strdup(line);
            count++;
            line = strtok(NULL, "\n");
        }
    }
    
    if (logNum < 1 || logNum > count) {
        printf("\033[1;31mInvalid log number '%d', log %d doesn't exist\033[0m\n", logNum, logNum);
    } else {
        int isCommandValid = 1;

        char storeCommandinLogforLog[5000];
        strcpy(storeCommandinLogforLog, logs[count - (logNum)]);

        struct sigaction sa;
        sa.sa_handler = &handle_sigchld;
        sa.sa_flags = SA_RESTART | SA_NOCLDSTOP; // Restart interrupted syscalls, don't notify on stopped children
        sigemptyset(&sa.sa_mask);
        if (sigaction(SIGCHLD, &sa, 0) == -1) {
            perror_red("sigaction");
            exit(1);
        }
        
        checkProcess* process_list;
        int process_list_size = checkForegroundOrBackgroundProcess(logs[count - (logNum)], &process_list);
        char command[5000];
        strcpy(command, logs[count - (logNum)]);

        if( strstr(command, "|") != NULL ) {
            for(int i = 0; i < process_list_size; i++) {
                char *process_command = process_list[i].command;            // command to be executed
                char *process_delimiter = process_list[i].delimiter;        // delimiter to be used
                int process_flag_size = checkFlags(process_command, &process_list[i]);      // flags in the command

                handlePipe(process_command, process_delimiter, lastdir, initialDirectory, sleepDuration, clockIndex, display_process_list, config_list, configCommandsSize, logFile, &isCommandValid, isLogExit, pidOfBackgroundCommands, pidOfBackgroundCommandsIndex, background_processes);
                // add to log
                // add neccessary args
            }
        } else {
            if(strcmp (logs[count - (logNum)], "exit") == 0) {
                *isLogExit = 1;
            }
            handleCommands(process_list, process_list_size, lastdir, initialDirectory, sleepDuration, clockIndex, display_process_list, config_list, configCommandsSize, logFile, &isCommandValid, isLogExit, pidOfBackgroundCommands, pidOfBackgroundCommandsIndex, background_processes);
        }

        if(isCommandValid == 1) {
            createLog(filepath, storeCommandinLogforLog);
        }
    }

    for (int i = 0; i < count; i++) {
        free(logs[i]);
    }
    free(fileContent);
    close(fd);

    if(*isLogExit == 1) return 1;
    return 0;
}
