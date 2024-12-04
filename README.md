# C-Shell - A Custom Shell in C

Welcome to **C-Shell**, a lightweight custom shell written in C that brings powerful features to your terminal. This shell supports advanced functionalities like command chaining, input/output redirection, piping, process control, and more, all while maintaining a user-friendly experience with color-coded prompts and helpful error messages.

## Features

- **Custom Prompt**: Displays the prompt in the format `<Username@SystemName:CurrentDirectory>` with color coding.
- **Command Execution**: Supports chaining commands with `;` and running background tasks with `&`.
- **Custom Commands**:
  - `hop`: Change directories with valid paths.
  - `reveal`: Display files and directories with colored output.
  - `log`: Manage command history with options like `purge` and `execute`.
  - `proclore`: Display process information.
  - `seek`: Search files or directories with advanced filters.
  - `activities`: List active processes in lexicographical order.
  - `fg` and `bg`: Foreground and background process management.
  - `ping`: Send signals to processes.
  - `neonate`: Countdown timer with user control.
  - `iMan`: Fetch manual pages for commands.
- **I/O Redirection**: Handle input/output redirection (`>`, `>>`, `<`) seamlessly.
- **Pipes**: Support for piping commands (`|`) and combined redirection with pipes.
- **Process Control**: Background and foreground process management, signal handling, and status tracking.
- **.myshrc Support**: Configure aliases for commands at startup.

---

## Installation Guide

### Prerequisites

Ensure the following tools are installed on your system:
- **GCC**: To compile the shell.
- **Make**: For build automation (optional but recommended).

### Steps to Install

1. **Clone the Repository**  
   Open your terminal and run:  
   ```bash
   git clone https://github.com/your-username/C-Shell.git
   cd C-Shell
   ```

2. **Build the Shell** \
   Compile the source code using the provided Makefile:
    ```bash
    make
    ```
    If you don't have make, you can compile manually:
    ```bash
    gcc ./*.c
    ```

3. **Run the Shell**
   ```bash
   ./a.out
   ```

## Usage

### Launch the shell:
- 
    ```bash
    ./a.out
    ```
    Start entering commands just like in a standard terminal. \
    You can use built-in commands or system commands.

### Use advanced features like:

- 
    ### Command chaining:
    ```bash
    <command1> ; <command2>
    ```

- 
    ### Background execution:
    ```bash
    <command> &
    ```

- 
    ### Redirection and piping:
    ```bash
    <command> > <file>
    <command1> | <command2>
    ```

- 
    ### Explore custom commands:

    - #### Navigate with hop:
        ```bash
        hop /path/to/directory
        ```

    - #### View file details with reveal:
        ```bash
        reveal -l /path/to/file
        ```

    - #### View your command history with log:
      ``` bash
      log
      ```
      - Clear history with purge:
        ```bash
        log purge
        ```

      - Execute a command from your history with execute:
        ```bash
        log execute <commandIndex> 
        ```

    - #### View status of a process with proclore
      ``` bash
      proclore <pid>
      ```

    - #### Find a file or directory using seek
      ```bash
      seek <fileName/directoryName>
      ```
      - Add flag -f to only find files
      - Add flag -d to only find directories
      - Add flag -e to execute the unique file, if found, or jump to the unique directory, if found.
  
    - #### See the status of processes using activities.
      ```bash
      activities
      ```

    - #### Add your own custom functionalities by editing .myshrc.
    - #### Manage processes with fg, bg, or proclore.
    - #### Exit the shell anytime with exit or Ctrl+D.
    - #### Interrupt running programs with Ctrl+C
    - #### Move foreground processes to background with Ctrl+Z 
    - #### Get the pid of the most recently created process in youe system using neonate.
    - #### Get the MAN page for any Linux system command by using iMan
    ```bash
    iMan <commandName>
    ```


### Enjoy using C-Shell! 🚀