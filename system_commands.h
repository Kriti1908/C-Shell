#ifndef SYSTEM_COMMANDS_H
#define SYSTEM_COMMANDS_H

static int* static_pidOfBackgroundCommandsIndex = NULL;
static bgProcess* static_background_processes = NULL;
// static int is_foreground_function_running ;

void systemCommand(char* command, int* isCommandValid, char* delimiter, int sleepDuration[], int* clockIndex, char** process_list, char** pidOfBackgroundCommands, int* pidOfBackgroundCommandsIndex, bgProcess background_processes[]);
void handle_sigchld(int sig);
void set_background_process_data(bgProcess* processes, int* index);
void handle_sigtstp(int sig);

#endif