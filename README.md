[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/Qiz9msrr)
# mini-project-1-template

# Part A : Basic System Calls

## General Assumptions

- Length of any and all inputs will be less than 5000 characters.
- Color coding of prompt is done as in bash.

## Specification 1 : Display Requirement 

### Format
``` html
<Username@SystemName:CurrentDirectory>
```
### Assumptions
- Username will be lesser than 5000 characters.
- CurrentDirectory will be lesser than 5000  characters.

## Specification 2 : Input Requirement 

### Format
``` html
<Username@SystemName:CurrentDirectory> Command1 ; Command2 &
```
### Assumptions
- Any command entered will be lesser than 5000 characters.
- '&' will come only after system commands.

## Specification 3 : Hop

### Format
``` html
hop <path> 
```
### Assumptions
- Paths will be valid, else suitable error messages will be displayed.
- Path will not contain any spaces.
- Absolute paths will begin with /

## Specification 4 : Reveal

### Format
``` html
reveal <flags> <path/name>
```
### Assumptions
- If any flag apart from -l and -a are added, then that case is NOT considered an error as it is not considered an error in bash.
- Only 1 path will be specified and two space separated paths are not supported.
- Blue color is used for directories, green for executables and white for the other files.


## Specification 5 : Log

### Format
```html
log 
log purge
log execute <index>
```
### Assumptions
- log is printed oldest first but executed newest first.
- If last log matches the new command, the new command isn't added to the file.
- If a line of command contains even one log, it will not be added to the log gile.
- During log execute, the command executed is added to the file, provided it is not the most recent command in the file.


## Specification 6 : System Commands

### Format
```html
<system command> <arguements>
```
### Assumptions
- Quotes will not be used for echo etc commands.
- Process ID of background process gets printed and status of completion as soon as they get executed.
- Any foreground process which takes longer than 2 seconds to run is included in the next prompt, but not in any prompt after that.
- Background processes are implemented only on system commands.
- Sleep time in prompt is rounded down to nearest integer.


## Specification 7 : Proclore

### Format
```html
proclore <pid>
```
### Assumptions
- If pid arguement is not passed, then automatically proclore will be executed for the running program ie the shell itself.
- If we don't have access to the process whose pid is entered, appropriate error messages will be displayed.
- A process is foreground process if its group id is the same as that of our terminal.
- ' + ' is printed after process status for foreground processes and not for background processes.
- If process with entered PID does not exist, then for each of the asked fields (status etc) suitable error messages are printed. The group id in this case is returned as -1.


## Specification 8 : Seek

### Format
```html
seek <flags> <search> <target_directory>
```
### Assumptions
- If both -d and -f flags are entered, that is an error and suitable error message is printed.
- Any file/directory whose prefix substring matches the search will be displayed.
- Only 1 path will be specified and two space separated paths are not supported.
- Blue color is used for directories, green for files.
- If -e flag is entered but more than one files/directories are found then it simply lists then out.
- If no files are found of the given name then appropriate message is displayed.
- if -d and -e flags are together and only 1 directory is found, so we call the hop() function.
- if -e flag is present with either -d or -f and multiple files are found, then -e flag has no effect.
- if -f and -e flags are together and only 1 file is found, so we print the contents of that file.


------------------------------------------------------------

# Part B : Processes, Files and Misc.

## Specification 9 : .myshrc 

### Assumptions
- All comments will begin with // and conversly any line starting with // will be considered as a comment.
- Only alias part of the specification has been implemented
- Alias given will be for a valid command.
- Any command given after an aliased command will not be executed.
- If alias is not the first command in a given list of commands, it will not execute as expected.

## Specification 10 : I/O Redirection

### Format
``` html
command > file
command >> file
command < file
```

### Assumptions
- Only valid symbols will be give as redirection symbols, ie, only `>`, `>>` and `<`.
- All redirection symbols will have spaces on both their left and their left.
- `>` overwrites content of the file and `>>` appends to the end of file.
- `<` takes input from the file.
- `&` cannot appear immediately before or after any redirection symbol.
- For opening, reading from or writing a file relative paths are NOT supported and only absolute paths are supported.

## Specification 11 : Pipes

### Format
``` html
command1 | command2
```
### Assumptions
- Each pipe `|` in the command will have a space to both its left and right.
- `&` cannot appear immediately before or after a `|`.
- For opening, reading from or writing a file relative paths are NOT supported and only absolute paths are supported.


## Specification 12 : Pipes & Redirection

### Format
``` html
command1 | command2 > file 
```
### Assumptions
- `&` cannot appear immediately before or after a `|` or any of the redirection symbols.
- Command entered will be valid and within scope of reson.
- For opening, reading from or writing a file relative paths are NOT supported and only absolute paths are supported.


## Specification 13 : Activities

### Format
``` html
activities
```
### Assumptions
- Output is displayed by lexicological ordering of process name.
- In case two or more processes have same name, the process with lower pid will be displayed on top.


## Specification 14 : Signals

### Format
``` html
ping <pid> <signal_no>
```
### Assumptions
- Signal no will be considered after taking modulo with 32.
- In case of signal 15, the process is gracefully terminated and removed from list of ongoing background processes.
- In case of Ctrl+C, any foreground process running at the time at which Ctrl+C is invoked will be gracefully terminated. Background processes will not be affected.
- In case of Ctrl+D, any backround process running at the time at which Ctrl+D is invoked will be gracefully terminated. In case a foreground process is active when Ctrl+D is invoked, the Ctrl+D will take effect after that foreground process finishes its execution.
- In case of Ctrl+Z, any foregroung process running at the time at which Ctrl+Z is stopped and sent to the background.


## Specification 15 : fg & bg

### Format
``` html
fg <pid>
bg <pid>
```
### Assumptions
- bg will convert the status of process with given PID to running but the process will remani in background.
- fg will convert the process with given PID to a foreground process.
- In case a process which was converted to foreground by fg, the time taken will be calculated from the moment they came to foreground.


## Specification 16 : Neonate

### Format
``` html
neonate -n <time>
```
### Assumptions
- time will always be >= 0.
- User will press x to quit.


## Specification 17 : iMan

### Format
``` html
iMan <command_name>
```
### Assumptions
- Command entered will be present in the man website.
- A few html tags will be visible above and below the required segment.






