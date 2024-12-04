#include "headers.h"
#include "proclore.h"

void proclore(char* homeDIR, pid_t pid, int terminalGroupID) {
    
    char path[5000], status;
    ssize_t len;
    FILE *fp;

    printf("pid: %d\n", pid);
    
    pid_t group_id = getpgid(pid);
    
    snprintf(path, sizeof(path), "/proc/%d/stat", pid);         // Process Status
    fp = fopen(path, "r");
    if (fp) {
        fscanf(fp, "%*d %*s %c", &status);
        fclose(fp);
        if(group_id == terminalGroupID) {                       // if group id == teminal group id => foreground
            printf("Process Status: %c+\n", status);
        } else {
        printf("Process Status: %c\n", status);
        }
    } else {
        perror_red("Failed to open stat file");
    }

    printf("Process Group: %d\n", group_id);

    unsigned long virtualMemorySize;
    snprintf(path, sizeof(path), "/proc/%d/statm", pid);        // Virtual Memory Size
    fp = fopen(path, "r");
    if (fp) {
        fscanf(fp, "%lu", &virtualMemorySize);
        fclose(fp);
        printf("Virtual Memory: %lu pages\n", virtualMemorySize);
    } else {
        perror_red("Failed to open statm file");
    }

    char exe_path[5000];            // Executable Path
    snprintf(path, sizeof(path), "/proc/%d/exe", pid);
    len = readlink(path, exe_path, sizeof(exe_path) - 1);
    if (len != -1) {
        exe_path[len] = '\0';
        if(strncmp(exe_path, homeDIR, strlen(homeDIR)) == 0) {
            printf("Executable Path: ~%s\n", exe_path + strlen(homeDIR));
        } else {
            printf("Executable Path: %s\n", exe_path);
        }
        // printf("Executable Path: %s\n", exe_path);
    } else {
        perror_red("Failed to read executable path");
    }
}
