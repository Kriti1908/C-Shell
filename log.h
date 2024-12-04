#ifndef LOG_H
#define LOG_H

#include "config.h"
// void writeToFile(const char* filename, const char* content);
// void readFromFile(const char* filename);

void createLog(char* filepath, char* content);
void printLogs(char* filepath);
void purgeLogs(char* filepath);
int executeLog(char* filepath, int logNum, char* lastdir, char* initialDirectory, int* sleepDuration, int* clockIndex, char** display_process_list, config_process* config_list, int configCommandsSize, char* logFile, int* isLogExit, char** pidOfBackgroundCommands, int* pidOfBackgroundCommandsIndex, bgProcess background_processes[]);

#endif