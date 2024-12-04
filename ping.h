#ifndef PING_H
#define PING_H

static int should_terminate_function;
// static int ctrlD_flag;
// static int* static_pidOfBackgroundCommandsIndex = NULL;
// static bgProcess* static_background_processes = NULL;
// static int foreground_pid;

void send_signal_to_process(int pid, int signal_no);
void ping(char* pid_str, char* signal_no_str);
void send_sigint_to_foreground();
void handle_sigint(int sig);
void handle_sigint(int sig);
void send_sigint_to_foreground();
void set_foreground_function_running(int value, int pid);
void cleanup_and_exit();
void handle_eof(int sig);

void initialize_shell();
void set_process_in_foreground(pid_t pid);


#endif