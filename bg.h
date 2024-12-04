#ifndef BG_H
#define BG_H

#include "headers.h"

static bgProcess* static_background_processes;
static int* static_pidOfBackgroundCommandsIndex;

void setValuesforBg(bgProcess* processes, int* index);
void bg (int pid);

#endif