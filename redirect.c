#include "headers.h"
#include "redirect.h"

int* checkRedict(char* process_command) {
    char check[5000];
    strcpy(check, process_command);
    int* list = (int*) malloc(4*sizeof(int));               // 2 slots each for input and output
    list[0] = -1;                                           // Stdout (>)
    list[1] = -1;                                           // Stdin (<)
    list[2] = -1;                                           // Check type for stdout
    list[3] = -1;                                           // Check type for stdin

    char* token = strtok(check, " \t");
    while(token != NULL) {
        if(strcmp(token, ">") == 0 || strcmp(token, ">>") == 0) {
            char* filename = strtok(NULL, " \t");
            if (filename == NULL) {
                fprintf(stderr, RED "No filename specified for redirection" RESET "\n");
                return NULL;
            }
            int fd = open(filename, O_WRONLY | O_CREAT | 
                          (strcmp(token, ">") == 0 ? O_TRUNC : O_APPEND), 0644);
            if(fd == -1) {
                perror_red("open");
                return NULL;
            }
            list[0] = fd;    // Output redirection file descriptor
            list[2] = 1;     // Set to stdout redirection
        } 
        else if (strcmp(token, "<") == 0) {
            char* filename = strtok(NULL, " \t");
            if (filename == NULL) {
                fprintf(stderr, RED "No filename specified for redirection" RESET "\n");
                exit(1);
            }
            int fd = open(filename, O_RDONLY);
            if(fd == -1) {
                perror_red("open");
                return NULL;
            }
            list[1] = fd;    // Input redirection file descriptor
            list[3] = 2;     // Set to stdin redirection
        }
        token = strtok(NULL, " \t");
    }
    return list;
}