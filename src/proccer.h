/*
 * Copyright 2016 Dan Molik <dan@d3fy.net>
 *
 * This file is part of Proccer
 *
 * Shitty Little Server is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * Shitty Little Server is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with Proccer.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _PROCCER_H
#define _PROCCER_H

#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <sys/stat.h>
#include <stdio.h>
#include <pthread.h>


#define MAX_PIDS 1024


typedef struct {
	unsigned long long  utime;
	unsigned long long  stime;
	unsigned long long  cutime;
	unsigned long long  cstime;
	unsigned long long  gtime;
	unsigned long long  itime;

	unsigned long       threads;
	unsigned int        procs;
	unsigned long       fds;

	unsigned long       vmpeak;
	unsigned long       vmsize;
	unsigned long       vmrss;
	unsigned long       vmhwm;

} stats_;


typedef struct {
	unsigned int  pid;
	unsigned int  ppid;
	stats_       *stats;
	char         *cmd;
} process_;

int get_procs(process_ **processes);

#endif
