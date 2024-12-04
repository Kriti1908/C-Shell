#ifndef PIPE_H
#define PIPE_H

#include "headers.h"

void handlePipe(char* processCommand, char* delimiter, char* lastdir, char* initialDirectory, int sleepDuration[], int* clockIndex, char** display_process_list, config_process* config_list, int configCommandsSize, char* logFile, int* isCommandValid, int* isLogExit, char** pidOfBackgroundCommands, int* pidOfBackgroundCommandsIndex, bgProcess background_processes[]);

#endif