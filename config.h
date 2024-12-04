#ifndef CONFIG_H
#define CONFIG_H

typedef struct config_process {
    char keyword[5000];
    char command[5000];
} config_process;

int config(char* filepath, config_process* config_list);
void printConfig(config_process* config_list);

#endif
