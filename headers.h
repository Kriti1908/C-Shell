#ifndef HEADER_H
#define HEADER_H

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <pwd.h>
#include <dirent.h>
#include <limits.h>
#include <errno.h>
#include <sys/types.h>
#include <time.h>
#include <sys/time.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <grp.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <termios.h>

#define RED "\033[31m"
#define GREEN "\033[32m"
#define BLUE "\033[34m"
#define RESET "\033[0m"


// extern int ctrlD_flag;
// extern int ctrlC_flag;
// extern int foreground_pid;
// extern int should_terminate_function;

void perror_red(const char *message);
// struct sigaction sa;

typedef struct checkProcess {
    char* command;
    char* delimiter;
    char** flag;
} checkProcess;

typedef struct {
    struct dirent *entry;
    struct stat file_stat;
} dir_entry;

typedef struct {
    pid_t pid;
    char command[256];
    char state[10]; // "running" or "stopped"
} bgProcess;

extern char* pidOfBackgroundCommands[100];
extern int pidOfBackgroundCommandsIndex;
extern bgProcess background_processes[5000];
extern int is_foreground_function_running;
extern char* foregroundProcessforZ;


#endif