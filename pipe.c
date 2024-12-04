#include "headers.h"
#include "config.h"
#include "commands.h"
#include "input_req.h"

void handlePipe(char* processCommand, char* delimiter, char* lastdir, char* initialDirectory, int sleepDuration[], int* clockIndex, char** display_process_list, config_process* config_list, int configCommandsSize, char* logFile, int* isCommandValid, int* isLogExit, char** pidOfBackgroundCommands, int* pidOfBackgroundCommandsIndex, bgProcess background_processes[]) {
    char command[5000];
    strcpy(command, processCommand);
    strcat(processCommand, delimiter);
    // printf("process command before checking delimiter: %s\n", processCommand);

    if( command[0] == '|') {
        printf(RED "Invalid use of pipe\n" RESET);
        return;
    }
    if ( command[strlen(command-1)] == '|') {
        printf(RED "Invalid use of pipe\n" RESET);
        return;
    }

    char* CommandsArr[5000];
    int index = 0;
    char* saveptr;
    char* delimiterList[50];
    char* token = strtok_r(command, "|", &saveptr);
    // printf("%s\n", token);
    if(token == NULL) {
        printf(RED "Invalid use of pipe\n" RESET);
    }
    while (token != NULL) {
        while (*token == ' ') {                         // remove start space
            token++;  
        }
        char* end = token + strlen(token) - 1;
        while (end > token && *end == ' ') {            // remove end space
            end--;  
        }

        if (token[*end] == '|') {
            printf(RED "Invalid use of pipe\n" RESET);
        }

        *(end + 1) = '\0';  
        CommandsArr[index] = token;
        delimiterList[index++] = delimiter;
        token = strtok_r(NULL, "|", &saveptr);
    }
    // printf("index = %d\n", index);

    int pipeCount = index - 1;  
    int og_stdout = dup(STDOUT_FILENO);  
    int og_stdin = dup(STDIN_FILENO);    
    int input_fd = og_stdin;        

    if( strcmp (delimiter, "&") == 0) {             // if delim = &, make every command except last to be ;
        for(int i = 0; i < index; i++) {
            if(i != index-1)
                delimiterList[i] = ";";
        }     
    }

    for (int idx = 0; idx <= pipeCount; idx++) {
        int pipefd[2];

        if (idx < pipeCount) {
            if (pipe(pipefd) == -1) {
                perror("pipe");
                exit(1);
            }
        }

        if (idx < pipeCount) {
            dup2(pipefd[1], STDOUT_FILENO);
            close(pipefd[1]);
        } else {
            dup2(og_stdout, STDOUT_FILENO);
        }

        dup2(input_fd, STDIN_FILENO);
        if (input_fd != og_stdin) {
            close(input_fd);
        }

        strcpy(processCommand, CommandsArr[idx]);  
        // printf("in pipe: %s\n", processCommand);
        if( strcmp (processCommand, "") == 0) {
            printf(RED "Invalid use of pipe\n" RESET);
        }

        handlePipedCommands(processCommand, delimiterList[idx], config_list, configCommandsSize, lastdir, isCommandValid, isLogExit, logFile, initialDirectory, sleepDuration, clockIndex, display_process_list, pidOfBackgroundCommands, pidOfBackgroundCommandsIndex, background_processes);

        if (idx < pipeCount) {
            input_fd = pipefd[0];                   // save for next pipe segment
            close(pipefd[1]);  
        }
    }

    dup2(og_stdin, STDIN_FILENO);
    dup2(og_stdout, STDOUT_FILENO);

    close(og_stdout);
    close(og_stdin);
}