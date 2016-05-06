#include "proccer.h"

static char buf[8192];

static char * _pfile(int pid, char *file) /* {{{ */
{
	static char tmp[32];
	sprintf(tmp, "/proc/%d/%s", pid, file);
	return tmp;
}
/* }}} */

int get_procs(process_ **processes) /* {{{ */
{
	pid_t pids[MAX_PIDS];
	DIR *dp;
	struct dirent *ep;

	int count = 0;
	if ((dp = opendir ("/proc")) != NULL) {
		while ((ep = readdir (dp)))
			if (strspn(ep->d_name, "0123456789") == strlen(ep->d_name))
				pids[count++] = atoi(ep->d_name);
		(void) closedir (dp);
	} else {
		fprintf(stderr, "failed to /proc %s\n", strerror(errno));
		exit(1);
	}
	int current = 0;
	unsigned long long x;
	for (int i = 0; i < count; i++) {
		if (pids[i] == 2 || pids[i] == 1 || pids[i] == 0) continue;
		process_ *process = malloc(sizeof(process_));
		process->stats    = malloc(sizeof(stats_));
		process->stats->procs = 1;
		FILE *io = malloc(sizeof(FILE));
		if (!(io = fopen(_pfile(pids[i], "stat"), "r"))) {
			fprintf(stderr, "failed to open stat file for pid %d; %s\n", process->pid, strerror(errno));
			exit(1);
		}
		int counter = 3; // align with man proc
		while (fgets(buf, 8192, io) != NULL) {
			char *pch = strtok(buf, ")");
			while (pch != NULL) {
				pch = strtok(NULL, " ");
				     if (counter ==  3) process->stats->state  = pch[0];
				else if (counter ==  4) process->ppid          = strtoul(pch, NULL, 0);
				else if (counter == 14) process->stats->utime  = strtoul(pch, NULL, 0);
				else if (counter == 15) process->stats->stime  = strtoul(pch, NULL, 0);
				else if (counter == 16) process->stats->cutime = strtoul(pch, NULL, 0);
				else if (counter == 17) process->stats->cstime = strtoul(pch, NULL, 0);
				else if (counter == 43) process->stats->gtime  = strtoul(pch, NULL, 0);
				else if (counter == 44) process->stats->cgtime = strtoul(pch, NULL, 0);
				counter++;
			}
		}
		fclose(io);
		if (process->ppid == 2) continue;

		if (!(io = fopen(_pfile(pids[i], "status"), "r"))) {
			fprintf(stderr, "failed to open status file for pid %d; %s\n", pids[i], strerror(errno));
			exit(1);
		}
		while (fgets(buf, 8192, io) != NULL) {
			/* MemTotal:        6012404 kB\n */
			char *k, *v, *u, *e;

			k = buf; v = strchr(k, ':');
			if (!v || !*v) continue;

			*v++ = '\0';
			while (isspace(*v)) v++;
			u = strchr(v, ' ');
			if (u) {
				*u++ = '\0';
			} else {
				u = strchr(v, '\n');
				if (u) *u = '\0';
				u = NULL;
			}

			x = strtoul(v, &e, 10);
			if (*e) continue;

			if (u && *u == 'k')
				x *= 1024;

			     if (streq(k, "Pid"))                         process->pid   = x;
			else if (streq(k, "Threads"))                     process->stats->threads = x;
			else if (streq(k, "State"))                       process->stats->state = x;
			else if (streq(k, "VmPeak"))                      process->stats->vmpeak = x;
			else if (streq(k, "VmSize"))                      process->stats->vmsize = x;
			else if (streq(k, "VmHWM"))                       process->stats->vmhwm = x;
			else if (streq(k, "VmRss"))                       process->stats->vmrss = x;
			else if (streq(k, "voluntary_ctxt_switches"))     process->stats->vctx = x;
			else if (streq(k, "nonvoluntary_ctxt_switches"))  process->stats->ictx = x;
		}
		fclose(io);

		if (!(io = fopen(_pfile(pids[i], "cmdline"), "rb"))) {
			fprintf(stderr, "failed to open cmdline file for pid %d; %s\n", process->pid, strerror(errno));
			exit(1);
		}
		size_t len = 0;
		char *path = 0;
		if (getdelim(&path, &len, 0, io) == -1) {
			fclose(io);
			fprintf(stderr, "failed to read cmdline file for pid %d; %s\n", process->pid, strerror(errno));
			exit(1);
		}
		fclose(io);
		process->cmd = strdup(path);
		free(path);
		processes[current++]  = process;
	}
	for (int i = 0; i < current; i++) {
		if (processes[i]->ppid == 1) continue;
		process_ *process = processes[i];
		while (process->ppid != 1) {
			for (int j = 0; j < current; j++) {
				if (processes[j]->pid != process->ppid) continue;
				process = processes[j];
				break;
			}
			if (process->pid == processes[i]->pid) break;
			process->stats->procs++;
		}
	}
	return current;
}
/* }}} */
