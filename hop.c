#include "headers.h"
#include "hop.h"
#include "display_req.h"
#include "redirect.h"

// char lastdir[5000];  

int hop(char *path, char* lastdir) {

    char arg[5000];
    if(path!=NULL) {
        // while(path!=NULL) {
            // printf("path in hop : %s\n", path);
            // if (strcmp(path, ">") == 0 || strcmp(path, ">>") == 0 || strcmp(path, "<") == 0) {
            //     break;
            // }
            while(isspace(*path)) {
                path++;
            }
            if (*path == '\0') {
                return -1;
            }
            if(strcmp(path, "-") == 0) {
                char newpath[5000];
                strcpy(newpath, lastdir);
                char temp[5000];
                if (getcwd(temp, sizeof(temp)) != NULL) {
                    strcpy(lastdir, temp);
                } else {
                    perror_red("getcwd");
                }
                strcpy(arg, newpath);
            } else {
                char temp[5000];
                if (getcwd(temp, sizeof(temp)) != NULL) {
                    strcpy(lastdir, temp);
                } else {
                    perror_red("getcwd");
                }
                strcpy(arg, path);
            }

            char curdir[5000];
            char path_check[10000];

            if (!getcwd(curdir, sizeof curdir)) {
                *curdir = '\0';
            }
            if (arg == NULL) {
                char cwd[2000];
                strcpy(cwd, getCWD(cwd));
                snprintf(path_check, sizeof path_check, "%s%s", cwd, "");
                strcpy(arg, path_check);  
            }
            else if (*arg == '~') {            
                char cwd[2000];
                strcpy(cwd, getCWD(cwd));
                // printf("cwd: %s\n", cwd);
                if (arg[1] == '/' || arg[1] == '\0') {
                    snprintf(path_check, sizeof path_check, "%s%s", cwd, arg + 1);
                    strcpy(arg, path_check);
                } else {
                    fprintf(stderr, "\033[1;31msyntax not supported: %s\033[0m\n", arg);
                    return 1;
                }
            } else if (*arg != '/') {
                snprintf(path_check, sizeof path_check, "%s/%s", curdir, arg);
                strcpy(arg, path_check);
            }

            if (chdir(arg)) {
                fprintf(stderr, "\033[1;31mchdir: %s: %s\033[0m\n", arg, strerror(errno));
                return 1;
            }
            else {
                char newdir[5000];
                if (!getcwd(newdir, sizeof newdir)) {
                    *newdir = '\0';
                }
                printf("%s\n", newdir);
            }

            // path = strtok(NULL, " \t");
            // if(path == NULL) break;
        // }
    }
    else {                                                  // to handle case when no path after hop
        char cwd[2000];
        strcpy(cwd, getCWD(cwd));
        // printf("cwd: %s\n", cwd);
        if (chdir(cwd)) {
            fprintf(stderr, "\033[1;31mchdir: %s: %s\033[0m\n", cwd, strerror(errno));
            return 1;
        }
        else {
            char newdir[5000];
            if (!getcwd(newdir, sizeof newdir)) {
                *newdir = '\0';
            }
            printf("%s\n", newdir);
        }
    }   

    

    return 0;
}

void hopHelper(char* str, char* lastdir) {     // didnt use coz i cant remember why i implemented in the first place

    char storeCommandForRedirect[5000];
    strcpy(storeCommandForRedirect, str);

    int original_stdout = dup(STDOUT_FILENO);
    int original_stdin = dup(STDIN_FILENO);

    int* redirections = checkRedict(storeCommandForRedirect);
    if (redirections[0] != -1 && redirections[2] == 1) {
        dup2(redirections[0], STDOUT_FILENO);
        close(redirections[0]);
    }
    // Handle input redirection (<)
    if (redirections[1] != -1 && redirections[3] == 2) {
        dup2(redirections[1], STDIN_FILENO);
        close(redirections[1]);
    }

    char* token = strtok(str, " ");
    while(token != NULL) {
        // printf("Token: %s\n", token);
        if(strcmp(token, "<") == 0 || strcmp(token, ">") == 0 || strcmp(token, ">>") == 0) {
            break;
        }
        hop(token, lastdir);
        token = strtok(NULL, " ");
    }

    dup2(original_stdout, STDOUT_FILENO);
    close(original_stdout);
    dup2(original_stdin, STDIN_FILENO);
    close(original_stdin);

    free(redirections);

    return;
}
