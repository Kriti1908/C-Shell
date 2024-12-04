#include "headers.h"
#include "activities.h"
#include "system_commands.h"

#define MAX_PROCESSES 100

void update_process_state(pid_t pid, char* state) {
    char proc_path[256], buffer[1024];
    snprintf(proc_path, sizeof(proc_path), "/proc/%d/stat", pid);

    int fd = open(proc_path, O_RDONLY);
    if (fd == -1) {
        strcpy(state, "stopped");
        return;
    }

    read(fd, buffer, sizeof(buffer) - 1);
    close(fd);

    char* token = strtok(buffer, " ");
    for (int i = 0; i < 2; i++) token = strtok(NULL, " ");
    char process_state = token[0];

    if (process_state == 'R' || process_state == 'S') {
        strcpy(state, "running");
    } else if (process_state == 'T') {
        strcpy(state, "stopped");
    } else {
        strcpy(state, "stopped");
    }
}


int is_pid_in_list(int pid, char **pid_list, int pid_count) {
    for (int i = 0; i < pid_count; ++i) {
        char pid_str[32];
        sprintf(pid_str, "%d", pid);
        if (strcmp(pid_str, pid_list[i]) == 0) {
            return 1;                                                                   // PID found in the list
        }
    }
    return 0;                                                                           // PID not found in the list
}


void print_background_processes(char **pid_list, bgProcess background_processes[], int* background_count) {
    for (int i = 0; i < *background_count - 1; i++) {                                   // Sort by command name
        for (int j = i + 1; j < *background_count; j++) {
            if (strcmp(background_processes[i].command, background_processes[j].command) > 0) {
                bgProcess temp = background_processes[i];
                background_processes[i] = background_processes[j];
                background_processes[j] = temp;
            }
        }
    }

    // printf("Background processes:\n");
    for (int i = 0; i < *background_count; i++) {
        if (strcmp(background_processes[i].state, "terminated") != 0) {                                     // Skip terminated processes
            update_process_state(background_processes[i].pid, background_processes[i].state);               // Update the state before printing
            if (strcmp(background_processes[i].state, "terminated") != 0) {
                printf("%d : %s - %s\n", background_processes[i].pid, background_processes[i].command, background_processes[i].state);
            }
        }
    }
}

void sigchld_handler(int sig, int* background_count, bgProcess background_processes[]) {
    pid_t pid;
    int status;

    while ((pid = waitpid(-1, &status, WNOHANG | WUNTRACED)) > 0) {
        int i;
        for (i = 0; i < *background_count; i++) {
            if (background_processes[i].pid == pid) {
                if (WIFEXITED(status) || WIFSIGNALED(status)) {                                 // Mark process as terminated
                    strcpy(background_processes[i].state, "terminated");
                    printf("Process %d terminated\n", pid);
                } else if (WIFSTOPPED(status)) {
                    strcpy(background_processes[i].state, "stopped");
                } else if (WIFCONTINUED(status)) {
                    strcpy(background_processes[i].state, "running");
                }
                
                if (strcmp(background_processes[i].state, "terminated") == 0) {                 // Remove the terminated process from the list
                    for (int j = i; j < *background_count - 1; j++) {
                        background_processes[j] = background_processes[j + 1];
                    }
                    (*background_count)--;
                }
                break;
            }
        }
    }
}
