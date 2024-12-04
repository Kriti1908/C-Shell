#ifndef FG_H
#define FG_H

#include "headers.h"

static bgProcess* static_background_processes;
static int* static_pidOfBackgroundCommandsIndex;

void setValuesforFg(bgProcess* processes, int* index);
void fg(int pid, int sleepDuration[], int *clockIndex, char **process_list);

#endif