#include "headers.h"
#include "display_req.h"
#include "input_req.h"
#include "system_commands.h"
#include "hop.h"
#include "reveal.h"
#include "log.h"
#include "proclore.h"
#include "seek.h"
#include "config.h"
#include "redirect.h"
#include "activities.h"
#include "ping.h"
#include "bg.h"
#include "fg.h"
#include "iman.h"
#include "neonate.h"

void handleCommands(checkProcess* process_list, int process_list_size, char* lastdir, char* initialDirectory, int sleepDuration[], int* clockIndex, char** display_process_list, config_process* config_list, int configCommandsSize, char* logFile, int* isCommandValid, int* isLogExit, char** pidOfBackgroundCommands, int* pidOfBackgroundCommandsIndex, bgProcess background_processes[]) {
    for(int i = 0; i < process_list_size; i++) {
            char process_command[5000];                      // command to be executed
            strcpy(process_command, process_list[i].command);
            char *process_delimiter = process_list[i].delimiter;                        // delimiter to be used
            int process_flag_size = checkFlags(process_command, &process_list[i]);      // flags in the command

            // printf("process delimiter: %s\n", process_delimiter);

            // while(process_command[0] == ' ') {                                            // remove leading spaces
            //     (&process_command)++;
            // }
            char storeCommand[5000];
            strcpy(storeCommand, process_command);                                      // store command for log - everything not separated by delimiter

            char storeCommandForRedirect[5000];
            strcpy(storeCommandForRedirect, process_command);

            char storeProcessCommand[5000];
            // make another copy for pipe?

            char* commandType = strtok(process_command, " \t");                         // command type
            char* remaining = storeCommand + strlen(commandType);
            if(commandType == NULL || strcmp(commandType, "") == 0 || *commandType == '\0') {
                continue;
            }

            for(int j = 0; j < configCommandsSize; j++) {
                if(strcmp(commandType, config_list[j].keyword) == 0) {
                    char newCommand[5000];
                    strcpy(newCommand, config_list[j].command);
                    strcat(newCommand, " ");
                    strcat(newCommand, process_command+strlen(commandType));
                    strcpy(process_command, newCommand);
                    strcat(process_command, " ");
                    if(strcmp(remaining, "") != 0) {
                        strcat(process_command, remaining);
                    }
                    // printf("New Command: %s\n", process_command);
                    strcpy(storeProcessCommand, process_command);
                    // printf("process_command: %s\n", process_command);
                    commandType = strtok(process_command, " \t");                       // command type
                    if(commandType == NULL || strcmp(commandType, "") == 0 || *commandType == '\0') {
                        continue;
                    }
                    break;
                }
            }
            foregroundProcessforZ = commandType;
    
            if(strcmp(commandType, "hop") == 0) {
                char* path = remaining;
                if(strcmp(path, "") == 0) {
                    path = strtok(NULL, " \t");
                }
                
                hopHelper(path, lastdir);
            }

            else if ( strcmp(commandType, "reveal") == 0) {
                char* path = strtok(NULL, " \t");
                int original_stdout = dup(STDOUT_FILENO);
                int original_stdin = dup(STDIN_FILENO);
                
                int l = 0, a = 0;
                // printf("Path: %s\n", path);
                if(path == NULL) {
                    path = "./";
                }
                else {
                    while(path[0] == '-') {
                        if(strlen(path) == 1) {
                            path = lastdir;
                            break;
                        } else {
                            for(int flag_index=1; flag_index<strlen(path); flag_index++) {
                                if(path[flag_index] == 'l') l=1;
                                if(path[flag_index] == 'a') a=1;
                                if(l && a) break;
                            }
                            path = strtok(NULL, " \t");
                            // printf("temp path: %s\n", path);
                            if( path == NULL || strcmp(path, ">") == 0 || strcmp(path, ">>") == 0 || strcmp(path, "<") == 0) {
                                path = NULL;
                            }
                            if(path == NULL) {
                                path = "./";
                                break;
                            }
                            while(path[0] == ' ') {
                                path++;
                            }
                        }
                    }
                    // printf("path: %s\n", path);
                    if( strcmp (path , "<") == 0 || strcmp (path, ">>") == 0 || strcmp (path, ">") == 0 || strcmp(path, "|") == 0) {
                        path = "./";
                    }  

                }
    
                int* redirections = checkRedict(storeCommandForRedirect);
                if (redirections[0] != -1 && redirections[2] == 1) {
                    dup2(redirections[0], STDOUT_FILENO);
                    close(redirections[0]);
                }
                if (redirections[1] != -1 && redirections[3] == 2) {
                    dup2(redirections[1], STDIN_FILENO);
                    close(redirections[1]);
                }
                
                for(int j = 0; j < process_flag_size; j++) {
                    if (process_list[i].flag[j] != NULL) {
                        if(strcmp(process_list[i].flag[j], "l") == 0) {
                            l = 1;
                        }
                        if(strcmp(process_list[i].flag[j], "a") == 0) {
                            a = 1;
                        }
                    }
                }

                // printf("final path: %s\n", path);
                reveal(path, l, a, lastdir);

                dup2(original_stdout, STDOUT_FILENO);
                close(original_stdout);
                dup2(original_stdin, STDIN_FILENO);
                close(original_stdin);

                free(redirections);
            }

            else if ( strcmp(commandType, "log") == 0 ) {
                *isCommandValid = 0;
                int isExit = 0;
                
                char* arguements = strtok(NULL, " \t");
                if( arguements != NULL) {
                    if( strcmp(arguements, ">") == 0 || strcmp(arguements, ">>") == 0 || strcmp(arguements, "<") == 0) {
                        arguements = NULL;
                    }
                }
                char* logNumber = NULL;
                if(arguements != NULL) {
                    logNumber = strtok(NULL, " \t");
                }

                int original_stdout = dup(STDOUT_FILENO);
                int original_stdin = dup(STDIN_FILENO);

                int* redirections = checkRedict(storeCommandForRedirect);
                if (redirections[0] != -1 && redirections[2] == 1) {
                    dup2(redirections[0], STDOUT_FILENO);
                    close(redirections[0]);
                }
                if (redirections[1] != -1 && redirections[3] == 2) {
                    dup2(redirections[1], STDIN_FILENO);
                    close(redirections[1]);
                }             

                if(arguements == NULL) {
                    printLogs(logFile);
                }
                
                else if(strcmp(arguements, "purge") == 0) {
                    purgeLogs(logFile);
                }
                
                else if( strcmp(arguements, "execute") ==0 ) {
                    
                    if(logNumber == NULL) {
                        printf("Please enter the log number\n");
                    } else {
                        int logNum = atoi(logNumber);
                        isExit = executeLog(logFile, logNum, lastdir, initialDirectory, sleepDuration, clockIndex, display_process_list, config_list, configCommandsSize, logFile, isLogExit, pidOfBackgroundCommands, pidOfBackgroundCommandsIndex, background_processes);
                        if(isExit == 1) {
                            *isLogExit = 1;
                            break;
                        }
                    }
                }
                
                else {
                    printf("\033[1;31mInvalid command\033[0m\n");
                }

                dup2(original_stdout, STDOUT_FILENO);
                close(original_stdout);
                dup2(original_stdin, STDIN_FILENO);
                close(original_stdin);

                free(redirections);
            }

            else if ( strcmp(commandType, "proclore") == 0 ) {
                int terminalPID = getpid();
                int terminalGroup = getpgid(terminalPID);

                char* pid = strtok(NULL, " \t");
                if(pid != NULL) {
                    if (strcmp(pid, ">") == 0 || strcmp(pid, ">>") == 0 || strcmp(pid, "<") == 0) {
                        pid = NULL;
                    }
                }

                int original_stdout = dup(STDOUT_FILENO);
                int original_stdin = dup(STDIN_FILENO);

                int* redirections = checkRedict(storeCommandForRedirect);
                if (redirections[0] != -1 && redirections[2] == 1) {
                    dup2(redirections[0], STDOUT_FILENO);
                    close(redirections[0]);
                }
                if (redirections[1] != -1 && redirections[3] == 2) {
                    dup2(redirections[1], STDIN_FILENO);
                    close(redirections[1]);
                }

                int process_id;
                if( pid == NULL) {
                    process_id = getpid();
                } else {
                    process_id = atoi(pid);
                }
                proclore(initialDirectory, process_id, terminalGroup);

                dup2(original_stdout, STDOUT_FILENO);
                close(original_stdout);
                dup2(original_stdin, STDIN_FILENO);
                close(original_stdin);
                free(redirections);
            } 

            else if ( strcmp(commandType, "seek") == 0 ) {
                char* search = strtok(NULL, " \t");
                seek_helper(search, lastdir, process_flag_size, process_list, i, storeCommandForRedirect);
            }

            else if ( strcmp(commandType, "activities") == 0 ) {
                int original_stdout = dup(STDOUT_FILENO);
                int original_stdin = dup(STDIN_FILENO);

                int* redirections = checkRedict(storeCommandForRedirect);
                if (redirections[0] != -1 && redirections[2] == 1) {
                    dup2(redirections[0], STDOUT_FILENO);
                    close(redirections[0]);
                }
                if (redirections[1] != -1 && redirections[3] == 2) {
                    dup2(redirections[1], STDIN_FILENO);
                    close(redirections[1]);
                }
                print_background_processes(pidOfBackgroundCommands, background_processes, pidOfBackgroundCommandsIndex);
                dup2(original_stdout, STDOUT_FILENO);
                close(original_stdout);
                dup2(original_stdin, STDIN_FILENO);
                close(original_stdin);

                free(redirections);
            }

            else if ( strcmp(commandType, "ping") == 0 ) {
                char* pid_str = strtok( NULL, " ");
                if( pid_str == NULL) {
                    printf(RED "Invalid Syntax: PID missing!" RESET "\n");
                    continue;
                }
                char* signalNo_str = strtok( NULL, " ");
                if(signalNo_str == NULL) {
                    printf(RED "Invalid Syntax: Signal number missing!" RESET "\n");
                    continue;
                }
                int original_stdout = dup(STDOUT_FILENO);
                int original_stdin = dup(STDIN_FILENO);

                int* redirections = checkRedict(storeCommandForRedirect);
                if (redirections[0] != -1 && redirections[2] == 1) {
                    dup2(redirections[0], STDOUT_FILENO);
                    close(redirections[0]);
                }
                if (redirections[1] != -1 && redirections[3] == 2) {
                    dup2(redirections[1], STDIN_FILENO);
                    close(redirections[1]);
                }
                ping(pid_str, signalNo_str);
                dup2(original_stdout, STDOUT_FILENO);
                close(original_stdout);
                dup2(original_stdin, STDIN_FILENO);
                close(original_stdin);

                free(redirections);
            }

            else if ( strcmp(commandType, "fg") == 0 ) {
                char* pid_str = strtok( NULL, " ");
                if( pid_str == NULL) {
                    printf(RED "Invalid Syntax: PID missing!" RESET "\n");
                    continue;
                }
                int original_stdout = dup(STDOUT_FILENO);
                int original_stdin = dup(STDIN_FILENO);

                int* redirections = checkRedict(storeCommandForRedirect);
                if (redirections[0] != -1 && redirections[2] == 1) {
                    dup2(redirections[0], STDOUT_FILENO);
                    close(redirections[0]);
                }
                if (redirections[1] != -1 && redirections[3] == 2) {
                    dup2(redirections[1], STDIN_FILENO);
                    close(redirections[1]);
                }
                setValuesforFg(background_processes, pidOfBackgroundCommandsIndex);
                int pid = atoi(pid_str);
                fg(pid, sleepDuration, clockIndex, display_process_list);
                dup2(original_stdout, STDOUT_FILENO);
                close(original_stdout);
                dup2(original_stdin, STDIN_FILENO);
                close(original_stdin);

                free(redirections);
            }

            else if ( strcmp(commandType, "bg") == 0 ) {
                char* pid_str = strtok( NULL, " ");
                if( pid_str == NULL) {
                    printf(RED "Invalid Syntax: PID missing!" RESET "\n");
                    continue;
                }
                int original_stdout = dup(STDOUT_FILENO);
                int original_stdin = dup(STDIN_FILENO);

                int* redirections = checkRedict(storeCommandForRedirect);
                if (redirections[0] != -1 && redirections[2] == 1) {
                    dup2(redirections[0], STDOUT_FILENO);
                    close(redirections[0]);
                }
                if (redirections[1] != -1 && redirections[3] == 2) {
                    dup2(redirections[1], STDIN_FILENO);
                    close(redirections[1]);
                }
                setValuesforBg(background_processes, pidOfBackgroundCommandsIndex);
                int pid = atoi(pid_str);
                bg(pid);
                dup2(original_stdout, STDOUT_FILENO);
                close(original_stdout);
                dup2(original_stdin, STDIN_FILENO);
                close(original_stdin);

                free(redirections);
            }

            else if ( strcmp(commandType, "iMan") == 0 ) {
                char* commandForIman = strtok ( NULL, " ");
                if( commandForIman == NULL ) {
                    printf(RED "Invalid Syntax: Command name for iman missing!" RESET "\n");
                    continue;
                }
                int original_stdout = dup(STDOUT_FILENO);
                int original_stdin = dup(STDIN_FILENO);

                int* redirections = checkRedict(storeCommandForRedirect);
                if (redirections[0] != -1 && redirections[2] == 1) {
                    dup2(redirections[0], STDOUT_FILENO);
                    close(redirections[0]);
                }
                if (redirections[1] != -1 && redirections[3] == 2) {
                    dup2(redirections[1], STDIN_FILENO);
                    close(redirections[1]);
                }
                int iman_ret = iman(commandForIman);
                dup2(original_stdout, STDOUT_FILENO);
                close(original_stdout);
                dup2(original_stdin, STDIN_FILENO);
                close(original_stdin);

                free(redirections);
                // if( iman_ret ) {
                //     printf("iman returned unsuccesfully\n");
                // } else {
                //     printf("iman returned succesfully\n");
                // }
            }

            else if ( strcmp(commandType, "neonate") == 0 ) {
                char* args = strtok(NULL, " ");
                if(args == NULL) {
                    printf(RED "Syntax error!" RESET "\n");
                } else if ( args[0] == '-') {
                    if (strlen(args) == 1) {
                        printf(RED "Syntax error!" RESET "\n");
                    } else {
                        if ( args[1] == 'n' ) {
                            char* timestr = strtok(NULL, " ");
                            if (timestr == NULL) {
                                printf(RED "Syntax error!" RESET "\n");
                            } else {
                                int original_stdout = dup(STDOUT_FILENO);
                                int original_stdin = dup(STDIN_FILENO);

                                int* redirections = checkRedict(storeCommandForRedirect);
                                if (redirections[0] != -1 && redirections[2] == 1) {
                                    dup2(redirections[0], STDOUT_FILENO);
                                    close(redirections[0]);
                                }
                                if (redirections[1] != -1 && redirections[3] == 2) {
                                    dup2(redirections[1], STDIN_FILENO);
                                    close(redirections[1]);
                                }
                                neonate(timestr);
                                dup2(original_stdout, STDOUT_FILENO);
                                close(original_stdout);
                                dup2(original_stdin, STDIN_FILENO);
                                close(original_stdin);

                                free(redirections);
                            }
                        } else {
                            printf(RED "Syntax error!" RESET "\n");
                        }
                    }
                }
            }

            else if ( strcmp(commandType, "exit") == 0 ) {
                cleanup_and_exit();
            }

            else {
                systemCommand(storeCommand, isCommandValid, process_delimiter, sleepDuration, clockIndex, display_process_list, pidOfBackgroundCommands, pidOfBackgroundCommandsIndex, background_processes);
            }
        }
        if(*isLogExit == 1) return;
}




void handlePipedCommands(char* command, char* delimeter, config_process* config_list, int configCommandsSize, char* lastdir, int* isCommandValid, int* isLogExit, char* logFile, char* initialDirectory, int sleepDuration[], int* clockIndex, char** display_process_list, char** pidOfBackgroundCommands, int* pidOfBackgroundCommandsIndex, bgProcess background_processes[]) {
    while (*command == ' ') {
        command++;
    }

    char storeCommand[5000];
    strcpy(storeCommand, command);                      // store command for log - everything not separated by delimiter

    char storeCommandForRedirect[5000];
    strcpy(storeCommandForRedirect, command);

    char storeProcessCommand[5000];

    char* commandType = strtok(command, " \t");           // command type
    char* remaining = storeCommand + strlen(commandType);
    if(commandType == NULL || strcmp(commandType, "") == 0 || *commandType == '\0') {
        return;
    }

    for(int j = 0; j < configCommandsSize; j++) {
        if(strcmp(commandType, config_list[j].keyword) == 0) {
            char newCommand[5000];
            strcpy(newCommand, config_list[j].command);
            strcat(newCommand, " ");
            strcat(newCommand, command+strlen(commandType));
            strcpy(command, newCommand);
            strcat(command, " ");
            if(strcmp(remaining, "") != 0) {
                strcat(command, remaining);
            }
            strcpy(storeProcessCommand, command);
            commandType = strtok(command, " \t");                                       // command type
            if(commandType == NULL || strcmp(commandType, "") == 0 || *commandType == '\0') {
                continue;
            }
            break;
        }
    }

    if(strcmp(commandType, "hop") == 0) {
        char* path = remaining;
        if(strcmp(path, "") == 0) {
            path = strtok(NULL, " \t");
        }
        hopHelper(path, lastdir);
    }

    else if ( strcmp(commandType, "reveal") == 0) {
        char* path = strtok(NULL, " \t");
        int original_stdout = dup(STDOUT_FILENO);
        int original_stdin = dup(STDIN_FILENO);
        
        int l = 0, a = 0;
        printf("Path: %s\n", path);
        if(path == NULL) {
            path = "./";
        }
        else {
            while(path[0] == '-') {
                if(strlen(path) == 1) {
                    path = lastdir;
                    break;
                } else {
                    for(int flag_index=1; flag_index<strlen(path); flag_index++) {
                        if(path[flag_index] == 'l') l=1;
                        if(path[flag_index] == 'a') a=1;
                        if(l && a) break;
                    }
                    path = strtok(NULL, " \t");
                    // printf("temp path: %s\n", path);
                    if( path == NULL || strcmp(path, ">") == 0 || strcmp(path, ">>") == 0 || strcmp(path, "<") == 0) {
                        path = NULL;
                    }
                    if(path == NULL) {
                        path = "./";
                        break;
                    }
                    while(path[0] == ' ') {
                        path++;
                    }
                }
            }
            printf("Path: %s\n", path);
            if( strcmp (path , "<") == 0 || strcmp (path, ">>") == 0 || strcmp (path, ">") == 0 ) {
                path = "./";
            }  

        }

        int* redirections = checkRedict(storeCommandForRedirect);
        if (redirections[0] != -1 && redirections[2] == 1) {
            dup2(redirections[0], STDOUT_FILENO);
            close(redirections[0]);
        }
        if (redirections[1] != -1 && redirections[3] == 2) {
            dup2(redirections[1], STDIN_FILENO);
            close(redirections[1]);
        }

        // printf("final path: %s\n", path);
        reveal(path, l, a, lastdir);

        dup2(original_stdout, STDOUT_FILENO);
        close(original_stdout);
        dup2(original_stdin, STDIN_FILENO);
        close(original_stdin);

        free(redirections);
    }

    else if ( strcmp(commandType, "log") == 0 ) {
        *isCommandValid = 0;
        int isExit = 0;
        
        char* arguements = strtok(NULL, " \t");
        if( arguements != NULL) {
            if( strcmp(arguements, ">") == 0 || strcmp(arguements, ">>") == 0 || strcmp(arguements, "<") == 0) {
                arguements = NULL;
            }
        }
        char* logNumber = NULL;
        if(arguements != NULL) {
            logNumber = strtok(NULL, " \t");
        }

        int original_stdout = dup(STDOUT_FILENO);
        int original_stdin = dup(STDIN_FILENO);

        int* redirections = checkRedict(storeCommandForRedirect);
        if (redirections[0] != -1 && redirections[2] == 1) {
            dup2(redirections[0], STDOUT_FILENO);
            close(redirections[0]);
        }
        if (redirections[1] != -1 && redirections[3] == 2) {
            dup2(redirections[1], STDIN_FILENO);
            close(redirections[1]);
        }             

        if(arguements == NULL) {
            printLogs(logFile);
        }
        
        else if(strcmp(arguements, "purge") == 0) {
            purgeLogs(logFile);
        }
        
        else if( strcmp(arguements, "execute") ==0 ) {
            
            if(logNumber == NULL) {
                printf("Please enter the log number\n");
            } else {
                int logNum = atoi(logNumber);
                isExit = executeLog(logFile, logNum, lastdir, initialDirectory, sleepDuration, clockIndex, display_process_list, config_list, configCommandsSize, logFile, isLogExit, pidOfBackgroundCommands, pidOfBackgroundCommandsIndex, background_processes);
                if(isExit == 1) {
                    *isLogExit = 1;
                    return;
                }
            }
        }
        
        else {
            printf("\033[1;31mInvalid command\033[0m\n");
        }

        dup2(original_stdout, STDOUT_FILENO);
        close(original_stdout);
        dup2(original_stdin, STDIN_FILENO);
        close(original_stdin);

        free(redirections);
    }

    else if ( strcmp(commandType, "proclore") == 0 ) {
        int terminalPID = getpid();
        int terminalGroup = getpgid(terminalPID);

        char* pid = strtok(NULL, " \t");
        if(pid != NULL) {
            if (strcmp(pid, ">") == 0 || strcmp(pid, ">>") == 0 || strcmp(pid, "<") == 0) {
                pid = NULL;
            }
        }

        int original_stdout = dup(STDOUT_FILENO);
        int original_stdin = dup(STDIN_FILENO);

        int* redirections = checkRedict(storeCommandForRedirect);
        if (redirections[0] != -1 && redirections[2] == 1) {
            dup2(redirections[0], STDOUT_FILENO);
            close(redirections[0]);
        }
        if (redirections[1] != -1 && redirections[3] == 2) {
            dup2(redirections[1], STDIN_FILENO);
            close(redirections[1]);
        }

        int process_id;
        if( pid == NULL) {
            process_id = getpid();
        } else {
            process_id = atoi(pid);
        }
        proclore(initialDirectory, process_id, terminalGroup);

        dup2(original_stdout, STDOUT_FILENO);
        close(original_stdout);
        dup2(original_stdin, STDIN_FILENO);
        close(original_stdin);
        free(redirections);
    } 

    else if ( strcmp(commandType, "seek") == 0 ) {
        // printf("%s\n", storeCommand);
        checkProcess* seekprocess_list;                                 
        int process_list_size = checkForegroundOrBackgroundProcess(storeCommand, &seekprocess_list);  
        char* newprocess = seekprocess_list[0].command;
        int process_flag_size = checkFlags(newprocess, &seekprocess_list[0]);
        char* typeOfCommand = strtok(newprocess, " \t");
        char* search = strtok(NULL, " \t");
        seek_helper(search, lastdir, process_flag_size, seekprocess_list, 0, storeCommandForRedirect);
    }

    else if ( strcmp(commandType, "activities") == 0 ) {
        int original_stdout = dup(STDOUT_FILENO);
        int original_stdin = dup(STDIN_FILENO);

        int* redirections = checkRedict(storeCommandForRedirect);
        if (redirections[0] != -1 && redirections[2] == 1) {
            dup2(redirections[0], STDOUT_FILENO);
            close(redirections[0]);
        }
        if (redirections[1] != -1 && redirections[3] == 2) {
            dup2(redirections[1], STDIN_FILENO);
            close(redirections[1]);
        }
        print_background_processes(pidOfBackgroundCommands, background_processes, pidOfBackgroundCommandsIndex);
        dup2(original_stdout, STDOUT_FILENO);
        close(original_stdout);
        dup2(original_stdin, STDIN_FILENO);
        close(original_stdin);

        free(redirections);
    }

    else if ( strcmp(commandType, "ping") == 0 ) {
        char* pid_str = strtok( NULL, " ");
        if( pid_str == NULL) {
            printf(RED "Invalid Syntax: PID missing!" RESET "\n");
            return;
        }
        char* signalNo_str = strtok( NULL, " ");
        if(signalNo_str == NULL) {
            printf(RED "Invalid Syntax: Signal number missing!" RESET "\n");
            return;
        }
        int original_stdout = dup(STDOUT_FILENO);
        int original_stdin = dup(STDIN_FILENO);

        int* redirections = checkRedict(storeCommandForRedirect);
        if (redirections[0] != -1 && redirections[2] == 1) {
            dup2(redirections[0], STDOUT_FILENO);
            close(redirections[0]);
        }
        if (redirections[1] != -1 && redirections[3] == 2) {
            dup2(redirections[1], STDIN_FILENO);
            close(redirections[1]);
        }
        ping(pid_str, signalNo_str);
        dup2(original_stdout, STDOUT_FILENO);
                close(original_stdout);
                dup2(original_stdin, STDIN_FILENO);
                close(original_stdin);

                free(redirections);
    }

    else if ( strcmp(commandType, "fg") == 0 ) {
        char* pid_str = strtok( NULL, " ");
        if( pid_str == NULL) {
            printf(RED "Invalid Syntax: PID missing!" RESET "\n");
            return;
        }
        int original_stdout = dup(STDOUT_FILENO);
        int original_stdin = dup(STDIN_FILENO);

        int* redirections = checkRedict(storeCommandForRedirect);
        if (redirections[0] != -1 && redirections[2] == 1) {
            dup2(redirections[0], STDOUT_FILENO);
            close(redirections[0]);
        }
        if (redirections[1] != -1 && redirections[3] == 2) {
            dup2(redirections[1], STDIN_FILENO);
            close(redirections[1]);
        }
        setValuesforFg(background_processes, pidOfBackgroundCommandsIndex);
        int pid = atoi(pid_str);
        fg(pid, sleepDuration, clockIndex, display_process_list);
        dup2(original_stdout, STDOUT_FILENO);
        close(original_stdout);
        dup2(original_stdin, STDIN_FILENO);
        close(original_stdin);

        free(redirections);
    }

    else if ( strcmp(commandType, "bg") == 0 ) {
        char* pid_str = strtok( NULL, " ");
        if( pid_str == NULL) {
            printf(RED "Invalid Syntax: PID missing!" RESET "\n");
            return;
        }
        int original_stdout = dup(STDOUT_FILENO);
        int original_stdin = dup(STDIN_FILENO);

        int* redirections = checkRedict(storeCommandForRedirect);
        if (redirections[0] != -1 && redirections[2] == 1) {
            dup2(redirections[0], STDOUT_FILENO);
            close(redirections[0]);
        }
        if (redirections[1] != -1 && redirections[3] == 2) {
            dup2(redirections[1], STDIN_FILENO);
            close(redirections[1]);
        }
        setValuesforBg(background_processes, pidOfBackgroundCommandsIndex);
        int pid = atoi(pid_str);
        bg(pid);
        dup2(original_stdout, STDOUT_FILENO);
        close(original_stdout);
        dup2(original_stdin, STDIN_FILENO);
        close(original_stdin);

        free(redirections);
    }

    else if ( strcmp(commandType, "iMan") == 0 ) {
        char* commandForIman = strtok ( NULL, " ");
        if( commandForIman == NULL ) {
            printf(RED "Invalid Syntax: Command name for iman missing!" RESET "\n");
            return;
        }
        int original_stdout = dup(STDOUT_FILENO);
        int original_stdin = dup(STDIN_FILENO);

        int* redirections = checkRedict(storeCommandForRedirect);
        if (redirections[0] != -1 && redirections[2] == 1) {
            dup2(redirections[0], STDOUT_FILENO);
            close(redirections[0]);
        }
        if (redirections[1] != -1 && redirections[3] == 2) {
            dup2(redirections[1], STDIN_FILENO);
            close(redirections[1]);
        }
        int iman_ret = iman(commandForIman);
        dup2(original_stdout, STDOUT_FILENO);
        close(original_stdout);
        dup2(original_stdin, STDIN_FILENO);
        close(original_stdin);

        free(redirections);
        // if( iman_ret ) {
        //     printf("iman returned unsuccesfully\n");
        // } else {
        //     printf("iman returned succesfully\n");
        // }
    }

    else if ( strcmp(commandType, "neonate") == 0 ) {
        char* args = strtok(NULL, " ");
        if(args == NULL) {
            printf(RED "Syntax error!" RESET "\n");
        } else if ( args[0] == '-') {
            if (strlen(args) == 1) {
                printf(RED "Syntax error!" RESET "\n");
            } else {
                if ( args[1] == 'n' ) {
                    char* timestr = strtok(NULL, " ");
                    if (timestr == NULL) {
                        printf(RED "Syntax error!" RESET "\n");
                    } else {
                        int original_stdout = dup(STDOUT_FILENO);
                        int original_stdin = dup(STDIN_FILENO);

                        int* redirections = checkRedict(storeCommandForRedirect);
                        if (redirections[0] != -1 && redirections[2] == 1) {
                            dup2(redirections[0], STDOUT_FILENO);
                            close(redirections[0]);
                        }
                        if (redirections[1] != -1 && redirections[3] == 2) {
                            dup2(redirections[1], STDIN_FILENO);
                            close(redirections[1]);
                        }
                        neonate(timestr);
                        dup2(original_stdout, STDOUT_FILENO);
                        close(original_stdout);
                        dup2(original_stdin, STDIN_FILENO);
                        close(original_stdin);

                        free(redirections);
                    }
                } else {
                    printf(RED "Syntax error!" RESET "\n");
                }
            }
        }
    }

    else if ( strcmp(commandType, "exit") == 0 ) {
        cleanup_and_exit();
    }
    
    else {
        systemCommand(storeCommand, isCommandValid, delimeter, sleepDuration, clockIndex, display_process_list, pidOfBackgroundCommands, pidOfBackgroundCommandsIndex, background_processes);
    }
}
