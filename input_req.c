#include "headers.h"
#include "input_req.h"

checkProcess processes[1000];

int checkForegroundOrBackgroundProcess(char* str, checkProcess** process_list) {
    int pIndex = 0;

    char string_copy[5000];
    strcpy(string_copy, str);
    int index = 0;

    char *token = strtok(str, ";&");
    while (token != NULL) {

        checkProcess temp = {token, "", NULL};
        temp.flag = (char**)malloc(50 * sizeof(char*));
        int flag_size = checkFlags(token, &temp);

        char *next = strtok(NULL, ";&");

        if (next != NULL) {
            if (index != 0) index += 1;
            index += strlen(token);
            char delimiter = string_copy[index];
            if (delimiter == ';') {
                temp.delimiter = ";";
            } else if (delimiter == '&') {
                temp.delimiter = "&";
            }
        } else {
            if (index != 0) index += 1;
            index += strlen(token);
            if (string_copy[index] == ';') {
                temp.delimiter = ";";
            } else if (string_copy[index] == '&') {
                temp.delimiter = "&";
            } else {
                temp.delimiter = ";";
            }
        }

        processes[pIndex++] = temp;

        token = next;
    }

    *process_list = processes;
    return pIndex;
}

int checkFlags(char* str, checkProcess* process) {
    int len = strlen(str), index = 0;
    for (int i = 0; i < len; i++) {
        if (str[i] == '-') {
            int j = i + 1;
            while (str[j] != ' ' && j < len) {
                if (str[j] != ' ') {
                    process->flag[index] = (char*)malloc(2 * sizeof(char));
                    process->flag[index][0] = str[j];
                    process->flag[index][1] = '\0';
                    index++;
                }
                j++;
            }
        }
    }
    // printf("command: %s, flags: \n", process->command);
    // for (int i = 0; i < index; i++) {
    //     printf("%s\n", process->flag[i]);
    // }
    return index;
}
