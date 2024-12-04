# ifndef display_req
# define display_req 

int is_inside_dir(const char *current_dir, const char *specific_dir);
int shellPrompt(char* initialDIR, int sleepDuration[], int sleepDurationSize, char** process_list);
char* getShellCWD();
char* getCWD();

#endif