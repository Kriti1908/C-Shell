#include "headers.h"
#include "fg.h"
#include "ping.h"

static bgProcess* static_background_processes;
static int* static_pidOfBackgroundCommandsIndex;

void setValuesforFg(bgProcess* processes, int* index) {
    static_background_processes = processes;
    static_pidOfBackgroundCommandsIndex = index;
}

void fg(int pid, int sleepDuration[], int *clockIndex, char **process_list) {
    int found = 0;

    for (int i = 0; i < *static_pidOfBackgroundCommandsIndex; i++) {
        if (static_background_processes[i].pid == pid) {
            found = 1;

            if (strcmp(static_background_processes[i].state, "stopped") == 0) {
                kill(pid, SIGCONT);
                printf("Resuming process %d (%s)\n", pid, static_background_processes[i].command);
            }

            set_foreground_function_running(1, pid);
            int status;
            struct timeval start_time, end_time;
            gettimeofday(&start_time, NULL);
            waitpid(pid, &status, WUNTRACED);
            gettimeofday(&end_time, NULL);

            double time_taken = (end_time.tv_sec - start_time.tv_sec) +
                                (end_time.tv_usec - start_time.tv_usec) / 1000000.0;

            // printf("time taken: %f\n", time_taken);
            if (time_taken > 2) {
                sleepDuration[*clockIndex] = (int)time_taken;
                process_list[*clockIndex] = strdup(static_background_processes[i].command);
                (*clockIndex)++;
            }

            if (WIFSTOPPED(status)) {
                printf("Foreground process %d stopped.\n", pid);
                strcpy(static_background_processes[i].state, "stopped");
            } else {
                printf("Foreground process %d finished.\n", pid);
                strcpy(static_background_processes[i].state, "terminated");
            }

            set_foreground_function_running(0, -1);
            break;
        }
    }
    if (!found) {
        printf(RED "No such process found with PID %d" RESET "\n", pid);
    }
}
