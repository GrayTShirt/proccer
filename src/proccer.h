#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <sys/stat.h>
#include <stdio.h>

#ifndef _PROCCER_H
#define _PROCCER_H

#define MAX_PIDS 1024

typedef struct {
	unsigned long long utime;
	unsigned long long stime;
	unsigned long long cutime;
	unsigned long long cstime;
} stats_;

typedef struct {
	unsigned int  pid;
	unsigned int  ppid;
	stats_       *stats;
	char         *cmd;
} process_;

// process_ pull_stat(int pid)

#endif
