#include "headers.h"
#include "display_req.h"

char ShellCWD[2000];

int is_inside_dir(const char *current_dir, const char *specific_dir) {
    if (strncmp(current_dir, specific_dir, strlen(specific_dir)) == 0) {        // Check if current_dir starts with specific_dir
        if (current_dir[strlen(specific_dir)] == '/' || current_dir[strlen(specific_dir)] == '\0') {
            return 1;
        }
    }
    return 0;
}

int shellPrompt(char* initialDIR, int sleepDuration[], int sleepDurationSize, char* process_list[]) {
    __uid_t uid = geteuid();                // gets process euid
    struct passwd *pw = getpwuid(uid);      // gets password uid
    // if(pw) {
    //     printf("%s\n", pw->pw_name);
    // }
    // else {
    //     printf("not found\n");
    // }

    // printf("sleep dur: %d\n", sleepDurationSize);

    char hostname[1000];
    int result;
    result = gethostname(hostname, 1000);
    if (result != 0)
    {
        perror_red("gethostname");
        return 1;
    }

    char cwd[2000];
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        perror_red("getcwd() error");
        return 1;
    }

    char prompt[5000];
    snprintf(prompt, sizeof(prompt), "<\033[32m%s@%s\033[0m:\033[0m", pw->pw_name, hostname);
    char* specific_dir;
    specific_dir = initialDIR;

    if (is_inside_dir(cwd, specific_dir)) {
        // printf("Inside %s\n", specific_dir);
        if(sleepDurationSize == 0 ) {
            snprintf(prompt + strlen(prompt), sizeof(prompt) - strlen(prompt), "\033[34m~%s\033[0m>", cwd + strlen(specific_dir));
        } else {
            snprintf(prompt + strlen(prompt), sizeof(prompt) - strlen(prompt), "\033[34m~%s\033[0m", cwd + strlen(specific_dir));
            int sum = 0;
            for(int i = 0; i < sleepDurationSize; i++) {
                sum += sleepDuration[i];
                if(i == sleepDurationSize-1) {
                    if(i == 0) {
                        snprintf(prompt + strlen(prompt), sizeof(prompt) - strlen(prompt), " %s: %ds>", process_list[i], sleepDuration[i]);
                    } else {
                        snprintf(prompt + strlen(prompt), sizeof(prompt) - strlen(prompt), " %s: %d, %ds>", process_list[i], sleepDuration[i], sum);
                    }

                } else {
                    snprintf(prompt + strlen(prompt), sizeof(prompt) - strlen(prompt), " %s: %d;", process_list[i], sleepDuration[i]);
                }
            }
            
        }
    } else {
        // printf("Not inside %s\n", specific_dir);
        if(sleepDurationSize == 0 ) {
            snprintf(prompt + strlen(prompt), sizeof(prompt) - strlen(prompt), "\033[34m%s\033[0m>", cwd);
        } else {
            snprintf(prompt + strlen(prompt), sizeof(prompt) - strlen(prompt), "\033[34m%s\033[0m", cwd);
            int sum = 0;
            for(int i = 0; i < sleepDurationSize; i++) {
                sum += sleepDuration[i];
                if(i == sleepDurationSize-1) {
                    if(i == 0) {
                        snprintf(prompt + strlen(prompt), sizeof(prompt) - strlen(prompt), " %s: %ds>", process_list[i], sleepDuration[i]);
                    } else {
                        snprintf(prompt + strlen(prompt), sizeof(prompt) - strlen(prompt), " %s: %d, %ds>", process_list[i], sleepDuration[i], sum);
                    }

                } else {
                    snprintf(prompt + strlen(prompt), sizeof(prompt) - strlen(prompt), " %s: %d;", process_list[i], sleepDuration[i]);
                }
            }
            
        }
    }
    printf("%s ", prompt);
    return 0;
}

char* getShellCWD() {
    if (getcwd(ShellCWD, sizeof(ShellCWD)) == NULL) {
        perror_red("getcwd(1) error");
        return NULL;
    }
    // printf("cwd in shwllCWD: %s\n", ShellCWD);
    return ShellCWD;
}

char* getCWD(char* cwd) {
    // char string[2000];
    strcpy(cwd, ShellCWD);
    return cwd;
}
