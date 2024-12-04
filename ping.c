#include "headers.h"
#include "ping.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

static int ctrlC_flag = 0;
static int ctrlD_flag = 0;
static int foreground_pid = -1;  // to track the PID of foreground process
// static int is_foreground_function_running = 0;

void send_signal_to_process(int pid, int signal_no) {
    int sig = signal_no % 32;

    union sigval value;
    value.sival_int = 0;  

    if (sigqueue(pid, sig, value) == -1) {
        if (errno == ESRCH) {
            printf(RED "No such process found" RESET "\n");
        } else {
            printf(RED "Failed to send signal: %s " RESET "\n", strerror(errno));
        }
    } else {
        printf("Signal %d sent to process %d\n", sig, pid);
    }

    if (kill(pid, 0) == -1) {  // to check if the process is still running after the signal
        if (errno == ESRCH) {
            printf(RED "Process %d does not exist (possibly terminated)." RESET "\n", pid);
        } else {
            printf(RED "Failed to check process status: %s" RESET "\n", strerror(errno));
        }
    } 
}

void ping(char* pid_str, char* signal_no_str) {
    int pid = atoi(pid_str);
    int signal_no = atoi(signal_no_str);
    if (pid <= 0 || signal_no < 0) {
        fprintf(stderr, RED "Invalid arguments: PID and signal number should be positive" RESET "\n");
        return;
    }
    send_signal_to_process(pid, signal_no);
}

// CTRL+C signal handler
void set_foreground_function_running(int value, int pid) {
    is_foreground_function_running = value;
    if (value) {
        foreground_pid = pid;  // Set the foreground process PID
    } else {
        foreground_pid = -1;  // Reset the foreground process PID
    }
}

void handle_sigint(int sig) {
    if (!ctrlC_flag) {
        if (is_foreground_function_running && foreground_pid != -1) {
            // printf("\nReceived SIGINT. Interrupting foreground process %d!\n", foreground_pid);
            kill(foreground_pid, SIGINT);  // Send SIGINT to the foreground process
            // should_terminate_function = 1; 
            foreground_pid = -1;
            is_foreground_function_running = 0;
        } else {
            printf("\nNo foreground process to interrupt.\n");
        }
        ctrlC_flag = 1;
    }
}

void send_sigint_to_foreground() {
    if (foreground_pid != -1) {
        int flag =1;
        for(int index = 0; index < pidOfBackgroundCommandsIndex; index++) {
            int temp = background_processes[index].pid;
            if(foreground_pid == temp) {
                flag = 0;
            }
        }
        if( flag ) {
            kill(foreground_pid, SIGINT);  // Send SIGINT to the foreground process
        }
    } else {
        printf("No foreground process to send SIGINT to.\n");
    }
}

