#ifndef INPUT_REQ_H
#define INPUT_REQ_H

int checkForegroundOrBackgroundProcess(char* str, checkProcess** process_list);
int checkFlags (char* str, checkProcess* process);
void printProcesses(checkProcess* process_list, int process_list_size);

#endif