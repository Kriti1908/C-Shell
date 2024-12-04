#include "headers.h"
#include "seek.h"
#include "display_req.h"
#include "hop.h"
#include "redirect.h"

void seek_helper(char* search, char* lastdir, int process_flag_size, checkProcess* process_list, int i, char* storeCommandForRedirect) {
    int d=0, e=0, f=0;
    while(search[0] == ' ') {
        search++;
    }
    while ( search[0] == '-') {
        if(search[1] != '\0') {
            if(search[1] == 'd') d =1;
            if(search[1] == 'e') e =1;
            if(search[1] == 'f') f =1;
        }
        search = strtok(NULL, " \t");
        while(search[0] == ' ') {
            search++;
        }
    }
    char* target = strtok(NULL, " \t");

    if(target != NULL) {
        if (strcmp(target, ">") == 0 || strcmp(target, ">>") == 0 || strcmp(target, "<") == 0) {
            target = NULL;
        }
    }

    int original_stdout = dup(STDOUT_FILENO);
    int original_stdin = dup(STDIN_FILENO);

    int* redirections = checkRedict(storeCommandForRedirect);
    if (redirections[0] != -1 && redirections[2] == 1) {
        dup2(redirections[0], STDOUT_FILENO);
        close(redirections[0]);
    }
    if (redirections[1] != -1 && redirections[3] == 2) {
        dup2(redirections[1], STDIN_FILENO);
        close(redirections[1]);
    }

    if(target == NULL) {
        target = "./";
    }
    else {
        while (target[0] == ' ') {
            target++;
        }

    }
    if (target[0] == '.') {
        char temp[5000];
        if (getcwd(temp, sizeof(temp)) != NULL) {
            strcat(temp, "/");
            strcat(temp, target);
            target = temp;
        } else {
            perror_red("getcwd");
            exit(1);
        }
    }
    if ( !( strcmp(target, "-") ) ) {
        if (*lastdir == '\0') {
            fprintf(stderr, "\033[1;31mNo previous directory\033[0m\n");
        } else {
            strcpy(target, lastdir);
            strcat(target, "/");
        } 
    } else if ( *target == '~' ) {
        char temp[5000];
        char tempPath[5001];
        strcpy(temp, getCWD(temp));
        if (target[1] == '/' || target[1] == '\0') {
            snprintf(tempPath, sizeof tempPath, "%s%s", temp, target + 1);
            target = tempPath;
            strcat(target, "/");
        } else {
            fprintf(stderr, "\033[1;31msyntax not supported: %s\033[0m\n", target);
        }
    }
    if(target[strlen(target)-1] != '/') {
        strcat(target, "/");
    }
    
    for(int j = 0; j < process_flag_size; j++) {
        if (process_list[i].flag[j] != NULL) {
            if(strcmp(process_list[i].flag[j], "d") == 0) {
                d = 1;
            }
            if(strcmp(process_list[i].flag[j], "e") == 0) {
                e = 1;
            }
            if(strcmp(process_list[i].flag[j], "f") == 0) {
                f = 1;
            }
        }
    }
    int count=0;
    char* executePath = NULL;
    if(d && f) {
        printf("\033[1;31mInvalid flags!\033[0m\n");
    } else {
        seek(search, target, target, &count, d, e, f, &executePath, lastdir, redirections);
    }
    struct stat statbuf;
    if(e && count == 1) {
        if(stat(executePath, &statbuf) == -1) {
            perror_red("stat");
        }

        if( S_ISDIR(statbuf.st_mode) ) {
            hop(executePath, lastdir);
        } else if ( S_ISREG(statbuf.st_mode) ) {
            int fd = open(executePath, O_RDONLY);
            if (fd == -1) {
                if (errno == EACCES) {
                    fprintf(stderr, "\033[1;31mMissing permission for task! : %s\033[0m\n", executePath);
                } else {
                    perror_red("open");
                }
            } else {
                char buffer[5000];
                ssize_t bytesRead;
                while ((bytesRead = read(fd, buffer, sizeof(buffer))) > 0) {
                    write(STDOUT_FILENO, buffer, bytesRead);
                }
                if (bytesRead == -1) {
                    perror_red("read");
                }
                close(fd);
            }
        }  
    }
    if (count == 0) {
        printf("No match found!\n");
    }

    dup2(original_stdout, STDOUT_FILENO);
    close(original_stdout);
    dup2(original_stdin, STDIN_FILENO);
    close(original_stdin);
    free(redirections);
}


void seek(const char *searchFile, const char* homeDIR, const char *targetDIR, int* count, int d, int e, int f, char** executePath, char* lastdir, int* redirections) {
    // printf("inside seek\n");

    int original_stdout = dup(STDOUT_FILENO);
    int original_stdin = dup(STDIN_FILENO);
    if (redirections[0] != -1 && redirections[2] == 1) {
        dup2(redirections[0], STDOUT_FILENO);
        close(redirections[0]);
    }
    if (redirections[1] != -1 && redirections[3] == 2) {
        dup2(redirections[1], STDIN_FILENO);
        close(redirections[1]);
    }

    if( !d && !f) { d=1; f=1; }

    DIR *dir;
    struct dirent *entry;
    struct stat statbuf;
    char path[5000];

    if ((dir = opendir(targetDIR)) == NULL) {
        perror_red("opendir");
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        if(targetDIR[strlen(targetDIR)-1] == '/') {
            snprintf(path, sizeof(path), "%s%s", targetDIR, entry->d_name);
        }
        else {
            snprintf(path, sizeof(path), "%s/%s", targetDIR, entry->d_name);
        }
        
        if (stat(path, &statbuf) == -1) {
            perror_red("stat");
            continue;
        }
        if (S_ISDIR(statbuf.st_mode)) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {      // Skip . and .. directories
                continue;
            }
            if ( d && (strncmp(entry->d_name, searchFile, strlen(searchFile)) == 0) ) {
                printf("\033[34m./%s\033[0m\n", path+strlen(homeDIR));
                (*count)++;
                if(e) {
                    if (*executePath == NULL) {
                        *executePath = strdup(path);
                    }
                }
            }
            seek(searchFile, homeDIR, path, count, d, e, f, executePath, lastdir, redirections);      //search in sub dirs
        } else {
            if ( f && (strncmp(entry->d_name, searchFile, strlen(searchFile)) == 0) ) {
                printf("\033[32m./%s\033[0m\n", path+strlen(homeDIR));
                (*count)++;
                if(e) {
                    if (*executePath == NULL) {
                        *executePath = strdup(path);
                    }
                }
            }
        }
    }
    // printf("executePath in seek: %s\n", executePath);
    closedir(dir);

    dup2(original_stdout, STDOUT_FILENO);
    close(original_stdout);
    dup2(original_stdin, STDIN_FILENO);
    close(original_stdin);
}


// add isredirect in seek 