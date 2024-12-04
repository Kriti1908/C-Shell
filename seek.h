#ifndef SEEK_H
#define SEEK_H

void seek_helper(char* search, char* lastdir, int process_flag_size, checkProcess* process_list, int i, char* storeCommandForRedirect);
void seek(const char *searchFile, const char* homeDIR, const char *targetDIR, int* count, int d, int e, int f, char** executePath, char* lastdir, int* redirections);

#endif