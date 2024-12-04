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
#include "commands.h"
#include "pipe.h"
#include "ping.h"

char lastdir[5000];  
int sleepDuration[5000];
char* display_process_list[5000];
config_process config_list[5000];
char* pidOfBackgroundCommands[100];
int pidOfBackgroundCommandsIndex = 0;
bgProcess background_processes[5000];
int is_foreground_function_running = 0;
char* foregroundProcessforZ;
static int should_terminate_function = 0;
static int foreground_pid;
static int ctrlD_flag = 0;


int main() 
{
    getShellCWD();                                                              // used in shellPrompt
    int configCommandsSize = config("./.myshrc", config_list);                  // read config file
    // printConfig(config_list);                                                // print config file

    char initialDirectory[5000];
    if(getcwd(initialDirectory, sizeof(initialDirectory)) == NULL) {
        perror_red("getcwd");
        return 1;
    }
    char logFile[5000];
    strcpy(logFile, initialDirectory);
    strcat(logFile, "/log.txt");
    
    char command[5000];
    char storeCommandForLog[5000];
    

    set_background_process_data(background_processes, &pidOfBackgroundCommandsIndex);
    signal(SIGCHLD, handle_sigchld);
    signal(SIGINT, handle_sigint);
    signal(SIGQUIT, handle_eof);
    signal(SIGTSTP, handle_sigtstp);


    int clockIndex = 0;
    struct sigaction sa;
    sa.sa_handler = &handle_sigchld;
    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;                                    // Restart interrupted syscalls, don't notify on stopped children
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGCHLD, &sa, 0) == -1) {
        perror_red("sigaction");
        exit(1);
    }


    int isLogExit = 0;

    while( ( strcmp( command , "exit" ) != 0) ) {
        if (ctrlD_flag) {
            cleanup_and_exit();
        }

        signal(SIGCHLD, handle_sigchld);
        signal(SIGINT, handle_sigint);
        signal(SIGQUIT, handle_eof);
        signal(SIGTSTP, handle_sigtstp);
        
        is_foreground_function_running = 0;
        should_terminate_function = 0;
        int process_pid = getpid();
        set_foreground_function_running(is_foreground_function_running, process_pid);
        
        shellPrompt(initialDirectory, sleepDuration, clockIndex, display_process_list);
        clockIndex=0;

        int isCommandValid = 1;                                                 // check to ensure log command is not added in log
        
        if (scanf(" %[^\n]s", command) == EOF) {
            ctrlD_flag = 1;
            continue;
        }

        char storeCommandForPipeDetection[5000];
        strcpy(storeCommandForPipeDetection, command);
        strcpy(storeCommandForLog, command);                                    // store command for log
        fflush(stdout);
        
        checkProcess* process_list;                                             // list of processes initited - sep by ;&
        int process_list_size = checkForegroundOrBackgroundProcess(command, &process_list);         // seperates processes based on ;& returns size of list

        // for(int i=0; i< process_list_size; i++) {
        //     printf("%s\n", process_list[i].command);
        // }

        foreground_pid = getpid();

        if( strstr(storeCommandForPipeDetection, "|") != NULL ) {
            is_foreground_function_running = 1;
            set_foreground_function_running(is_foreground_function_running, process_pid);
            for(int i = 0; i < process_list_size; i++) {
                char process_command[5000];                      // command to be executed
                strcpy(process_command, process_list[i].command);
                // printf("in pipe: %s....\n", process_command);
                char process_delimiter[5000];                 // delimiter to be used
                strcpy(process_delimiter, process_list[i].delimiter);
                // printf("del: %s\n", process_delimiter);
                int process_flag_size = checkFlags(process_command, &process_list[i]);      // flags in the command

                handlePipe(process_command, process_delimiter, lastdir, initialDirectory, sleepDuration, &clockIndex, display_process_list, config_list, configCommandsSize, logFile, &isCommandValid, &isLogExit, pidOfBackgroundCommands, &pidOfBackgroundCommandsIndex, background_processes);
                // add to log
                // add neccessary args
            }
        } else {
            is_foreground_function_running = 1;
            set_foreground_function_running(is_foreground_function_running, process_pid);
            handleCommands(process_list, process_list_size, lastdir, initialDirectory, sleepDuration, &clockIndex, display_process_list, config_list, configCommandsSize, logFile, &isCommandValid, &isLogExit, pidOfBackgroundCommands, &pidOfBackgroundCommandsIndex, background_processes);
        }

        if(isCommandValid == 1) {
            createLog(logFile, storeCommandForLog);
        }
        
        // printf("%d\n", isLogExit);
        if( strcmp ( command , "exit" ) == 0 ) {
            cleanup_and_exit();
        }
        if( isLogExit ) cleanup_and_exit();
        is_foreground_function_running = 0;
    }
    return 0;
}

// remove debug statements