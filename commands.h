#ifndef COMMANDS_H
#define COMMANDS_H

void handleCommands(checkProcess* process_list, int process_list_size, char* lastdir, char* initialDirectory, int sleepDuration[], int* clockIndex, char** display_process_list, config_process* config_list, int configCommandsSize, char* logFile, int* isCommandValid, int* isLogExit, char** pidOfBackgroundCommands, int* pidOfBackgroundCommandsIndex, bgProcess background_processes[]);
void handlePipedCommands(char* command, char* delimeter, config_process* config_list, int configCommandsSize, char* lastdir, int* isCommandValid, int* isLogExit, char* logFile, char* initialDirectory, int sleepDuration[], int* clockIndex, char** display_process_list, char** pidOfBackgroundCommands, int* pidOfBackgroundCommandsIndex, bgProcess background_processes[]);

#endif