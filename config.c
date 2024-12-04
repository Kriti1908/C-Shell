#include "headers.h"
#include "config.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

int config(char* filepath, config_process* config_list) {
    int fd = open(filepath, O_RDWR | O_CREAT | O_APPEND, 0644);
    if (fd == -1) {
        perror("open");
        exit(1);
    }

    off_t fileSize = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET); 
    char *fileContent = malloc(fileSize + 1); 
    if (fileContent == NULL) {
        perror("malloc");
        close(fd);
        exit(1);
    }

    ssize_t bytesRead = read(fd, fileContent, fileSize);
    if (bytesRead == -1) {
        perror("read");
        free(fileContent);
        close(fd);
        exit(1);
    }
    fileContent[bytesRead] = '\0';

    char* config[5000];
    int count = 0;

    char *line = strtok(fileContent, "\n");
    while (line != NULL) {
        config[count] = strdup(line);
        if (config[count] == NULL) {
            perror("strdup");
            free(fileContent);
            close(fd);
            exit(1);
        }
        count++;
        line = strtok(NULL, "\n");
    }

    int config_index = 0;
    for (int i = 0; i < count; i++) {
        line = config[i];
        char temp_line[5000];
        strncpy(temp_line, line, sizeof(temp_line) - 1);
        temp_line[sizeof(temp_line) - 1] = '\0'; 

        if (strncmp(temp_line, "//", 2) != 0) {
            char* word = strtok(temp_line, " \t");
            if (word != NULL) {
                if (word[strlen(word) - 1] != ')') { 
                    if(strcmp(word, "{") == 0) {            // to ignore content b/w curly brackets
                        word = strtok(NULL, " \t");
                        while(word != NULL) {
                            if(strcmp(word, "}") == 0) {
                                break;
                            }
                            word = strtok(NULL, " \t");
                        }
                        if(word == NULL) break;
                    }
                    
                    if (strcmp(word, "alias") == 0) {
                        // printf("%s\n", word);
                        word = strtok(NULL, " \t");
                    }
                    // printf("%s\n", word);
                    strncpy(config_list[config_index].keyword, word, sizeof(config_list[config_index].keyword) - 1);
                    config_list[config_index].keyword[sizeof(config_list[config_index].keyword) - 1] = '\0';

                    word = strtok(NULL, " \t");
                    while (word != NULL) {
                        // printf("command: %s\n", word);
                        if(strcmp(word, "=") == 0) {
                            char aliasedCommand[5000];
                            strcpy(aliasedCommand, "");
                            word = strtok(NULL, " \t");
                            while(word != NULL) {
                                // printf("new: %s\n", word);
                                // printf("line: %s\n", line);
                                strcat(aliasedCommand, word);
                                strcat(aliasedCommand, " ");
                                word = strtok(NULL, " \t");
                            }
                            strcpy(config_list[config_index].command, aliasedCommand);
                            config_list[config_index].command[sizeof(config_list[config_index].command) - 1] = '\0';
                            // printf("final command: %s\n", config_list[config_index].command);
                            break;
                        } else {
                            word = strtok(NULL, " \t");
                        }
                        
                    }
                    config_index++;
                }
            }
        }
    }

    for (int i = 0; i < count; i++) {
        free(config[i]);
    }
    free(fileContent);
    close(fd);

    return config_index;
}

void printConfig(config_process* config_list) {
    for (int i = 0; i < 5000; i++) {
        if (strcmp(config_list[i].keyword, "") == 0) {
            break;
        }
        printf("Keyword: %s\n", config_list[i].keyword);
        printf("Command: %s\n", config_list[i].command);
    }
}
