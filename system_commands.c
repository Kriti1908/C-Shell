#include "headers.h"
#include "system_commands.h"
#include "redirect.h"
#include "ping.h"

char *BackgroundCommands[100000000];
static int foreground_pid = -1;
static int ctrlD_flag = 0;
// int is_foreground_function_running;

void systemCommand(char *command, int *isCommandValid, char *delimiter, int sleepDuration[], int *clockIndex, char **process_list, char **pidOfBackgroundCommands, int *pidOfBackgroundCommandsIndex, bgProcess background_processes[])
{
    char storeCommandForRedirect[5000];
    strcpy(storeCommandForRedirect, command);

    char tokeniser[5000];
    if (strstr(command, "\'") != NULL)
    {
        strcpy(tokeniser, "\'\n");
    }
    else
    {
        strcpy(tokeniser, " \t\n");
    }
    char *commandType = strtok(command, tokeniser);
    char **args = (char **)malloc(1000 * sizeof(char *));
    int i = 0;

    while (commandType != NULL)
    {
        if (strcmp(commandType, ">") == 0 || strcmp(commandType, ">>") == 0 || strcmp(commandType, "<") == 0)
        {
            break;
        }
        while (*commandType == ' ')
        {
            commandType++;
        }
        while (commandType[strlen(commandType) - 1] == ' ')
        {
            commandType[strlen(commandType) - 1] = '\0';
        }
        args[i] = commandType;
        commandType = strtok(NULL, tokeniser);
        i++;
    }
    args[i] = NULL;

    // if (strcmp(args[0], "wc") == 0) {
    //     if(strcmp(args[1], "&") == 0) {
    //         args[1] = NULL;
    //     }
    // }

    int *redirections = checkRedict(storeCommandForRedirect);

    int pid = fork();
    if (pid < 0) {
        perror_red("fork");
    }
    
    else if (pid == 0) {
        if(strcmp (delimiter, "&") == 0 ) {
            setpgid(0, 0);
        }
        // Handle output redirection (>)
        if (redirections[0] != -1 && redirections[2] == 1)
        {
            dup2(redirections[0], STDOUT_FILENO);
            close(redirections[0]);
        }
        // Handle input redirection (<)
        if (redirections[1] != -1 && redirections[3] == 2)
        {
            dup2(redirections[1], STDIN_FILENO);
            close(redirections[1]);
        }

        // printf("%s\n", args[0]);
        // if(strcmp(args[0], "wc") == 0 ) {
        //     printf("dem\n");
        // }

        if (execvp(args[0], args) == -1)
        {
            *(isCommandValid) = 0;
            printf("\033[1;31mERROR: '%s' is not a valid command\033[0m\n", args[0]);
        }
        free(args);
        exit(1);
    }
    
    else {
        foreground_pid = pid;
        if (strcmp(delimiter, "&") != 0) {
            set_foreground_function_running(1, pid); 
            // printf("in syscom :%d\n", is_foreground_function_running);
            struct timeval start_time, end_time;
            gettimeofday(&start_time, NULL);
            // wait(NULL);
            int status;
            waitpid(pid, &status, WUNTRACED);                                                   // Use WUNTRACED to catch stopped processes

            if (WIFSTOPPED(status)) {\
                printf("\nForeground process %d stopped by signal.\n", pid);
            } else {
                // Process finished or exited normally
                set_foreground_function_running(0, -1);
            }
            gettimeofday(&end_time, NULL);

            double time_taken = (end_time.tv_sec - start_time.tv_sec) +
                                (end_time.tv_usec - start_time.tv_usec) / 1000000.0;

            // printf("time taken: %f\n", time_taken);
            if (time_taken > 2) {
                sleepDuration[*clockIndex] = (int)time_taken;
                process_list[*clockIndex] = strdup(args[0]);
                (*clockIndex)++;
            }
            

        } else {
            is_foreground_function_running = 0;
            set_foreground_function_running(is_foreground_function_running, pid);
            BackgroundCommands[pid] = strdup(args[0]);
            printf("%d\n", pid);
            char *pidString = (char *)malloc(1000 * sizeof(char));
            sprintf(pidString, "%d", pid);
            pidOfBackgroundCommands[(*pidOfBackgroundCommandsIndex)] = strdup(pidString);
            background_processes[*pidOfBackgroundCommandsIndex].pid = pid;
            strcpy(background_processes[*pidOfBackgroundCommandsIndex].command, args[0]);
            strcpy(background_processes[*pidOfBackgroundCommandsIndex].state, "running");
            (*pidOfBackgroundCommandsIndex)++;
        }
    }
    return;
}


void handle_sigchld(int sig)
{
    int status;
    pid_t pid;

    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        if (WIFEXITED(status) || WIFSIGNALED(status)) {
            printf("%s exited normally (%d)\n", BackgroundCommands[pid], pid);

            // Mark the background process as terminated
            for (int i = 0; i < *static_pidOfBackgroundCommandsIndex; i++) {
                if (static_background_processes[i].pid == pid) {
                    strcpy(static_background_processes[i].state, "terminated");
                    break;
                }
            }

            free(BackgroundCommands[pid]);
            BackgroundCommands[pid] = NULL;
        }
        fflush(stdout);
    }
    if (pid == -1 && errno != ECHILD) {
        perror("waitpid");
    }
}

void set_background_process_data(bgProcess *processes, int *index)
{
    static_background_processes = processes;
    static_pidOfBackgroundCommandsIndex = index;
}

void handle_eof(int sig) {
    if (is_foreground_function_running && foreground_pid != -1) {
        // printf("\nReceived EOF (Ctrl+D). Terminating foreground process %d!\n", foreground_pid);
        kill(foreground_pid, SIGKILL);                                                     // Send SIGKILL to the foreground process
        should_terminate_function = 1;
    } else {
        printf("\nNo foreground process to terminate.\n");
    }
    for (int i = 0; i < *static_pidOfBackgroundCommandsIndex; i++) {
        if (strcmp(static_background_processes[i].state, "running") == 0 ||
            strcmp(static_background_processes[i].state, "stopped") == 0) {
            // printf("Killing background process %d (%s)\n", static_background_processes[i].pid, static_background_processes[i].command);
            kill(static_background_processes[i].pid, SIGKILL);                             // Kill the background process
        }
    }
    ctrlD_flag = 1;                                                                        
}

void cleanup_and_exit() {
    if (ctrlD_flag != 1) {
        if (is_foreground_function_running && foreground_pid != -1) {
            printf("\nCleaning up and terminating foreground process %d...\n", foreground_pid);
            should_terminate_function = 1;  
            sleep(1);  
            kill(foreground_pid, SIGKILL); 
            printf("Foreground process %d terminated.\n", foreground_pid);
        } else {
            printf("\nNo foreground process to terminate.\n");
        }

        // Kill all background processes
        for (int i = 0; i < *static_pidOfBackgroundCommandsIndex; i++) {
            if (strcmp(static_background_processes[i].state, "running") == 0 ||
                strcmp(static_background_processes[i].state, "stopped") == 0) {
                printf("Killing background process %d (%s)\n", static_background_processes[i].pid, static_background_processes[i].command);
                kill(static_background_processes[i].pid, SIGKILL);  // Kill the background process
            }
        }
        printf("Logging out of shell.\n");
    }
    exit(0);  
}

void addBackgroundProcess(pid_t pid, char* commandName) {
    BackgroundCommands[pid] = strdup(commandName);
    // printf("%d\n", pid);
    char *pidString = (char *)malloc(1000 * sizeof(char));
    sprintf(pidString, "%d", pid);
    pidOfBackgroundCommands[(pidOfBackgroundCommandsIndex)] = strdup(pidString);
    background_processes[pidOfBackgroundCommandsIndex].pid = pid;
    strcpy(background_processes[pidOfBackgroundCommandsIndex].command, commandName);
    strcpy(background_processes[pidOfBackgroundCommandsIndex].state, "running");
    (pidOfBackgroundCommandsIndex)++;

}

void handle_sigtstp(int sig) {
    if (is_foreground_function_running && foreground_pid != -1) {
        // printf("\nReceived SIGTSTP (Ctrl+Z). Stopping foreground process %d!\n", foreground_pid);

        // Send SIGTSTP to the foreground process
        if (kill(foreground_pid, SIGTSTP) == 0) {
            // printf("Sent SIGTSTP to process %d.\n", foreground_pid);
            addBackgroundProcess(foreground_pid, foregroundProcessforZ);
        } else {
            perror("Failed to send SIGTSTP");
            return;
        }
    } else {
        printf("\nNo foreground process to stop.\n");
        return;
    }
    fflush(stdout);
}
