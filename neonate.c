#include "headers.h"
#include "neonate.h"

int max_pid() {                           // max val pid
    DIR *dir;
    struct dirent *entry;
    int maxPID = -1;

    dir = opendir("/proc");
    if (dir == NULL) {
        perror("opendir");
        return -1;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR && isdigit(entry->d_name[0])) {
            int pid = atoi(entry->d_name);
            if (pid > maxPID) {
                maxPID = pid;
            }
        }
    }

    closedir(dir);
    return maxPID;
}

void RawMode(int enable) {
    static struct termios oldt, newt;

    if (enable) {
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    } else {
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    }
}

void NonBlocking(int enable) {
    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    if (enable) {
        fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
    } else {
        fcntl(STDIN_FILENO, F_SETFL, flags & ~O_NONBLOCK);
    }
}


int xPress() {                              // detect x
    char ch;
    if (read(STDIN_FILENO, &ch, 1) == 1) {
        if (ch == 'x') {
            return 1;
        }
    }
    return 0;
}

int neonate(char* arg) {
    char* timeStr = strtok(arg, " ");
    int timePeriod = atoi(timeStr);
    if (timePeriod < 0) {
        fprintf(stderr, RED "Invalid timePeriod value.\n" RESET);
        return 1;
    }

    RawMode(1);  
    NonBlocking(1);

    while (1) {
        int recent_pid = max_pid();
        if (recent_pid != -1) {
            printf("%d\n", recent_pid);
        }

        for (int i = 0; i < timePeriod; i++) {
            if (xPress()) {
                // printf("...\n");
                RawMode(0);  
                NonBlocking(0);
                return 0;
            }
            sleep(1);  
        }
    }
    RawMode(0); 
    NonBlocking(0); 
    return 0;
}
