#ifndef ACTIVITIES_H
#define ACTIVITIES_H

#define PROC_DIR "/proc"
#define MAX_PROCESSES 100

void update_process_state(pid_t pid, char* state);
int is_pid_in_list(int pid, char **pid_list, int pid_count);
void print_background_processes(char **pid_list, bgProcess background_processes[], int* background_count);
void sigchld_handler(int sig, int* background_count, bgProcess background_processes[]);

#endif