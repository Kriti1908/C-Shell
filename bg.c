#include "headers.h"
#include "bg.h"

void setValuesforBg(bgProcess* processes, int* index) {
    static_background_processes = processes;
    static_pidOfBackgroundCommandsIndex = index;
}

void bg (int pid) {
    int i;
    int process_found = 0;

    for (i = 0; i < *static_pidOfBackgroundCommandsIndex; i++) {
        if (static_background_processes[i].pid == pid) {
            process_found = 1;
            
            if (strcmp(static_background_processes[i].state, "stopped") == 0) {
                if (kill(pid, SIGCONT) == -1) {
                    if (errno == ESRCH) {
                        printf(RED "No such process found" RESET "\n");
                    } else {
                        printf(RED "Failed to send signal: %s" RESET "\n", strerror(errno));
                    }
                } else {
                    printf("Process %d resumed in the background\n", pid);
                    strcpy(static_background_processes[i].state, "running");
                }
            } else {
                printf("Process %d is not stopped\n", pid);
            }
            break;
        }
    }
    if (!process_found) {
        printf(RED "No such process found" RESET "\n");
    }
}
