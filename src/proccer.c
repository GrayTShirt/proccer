#include "proccer.h"

static char buf[8192];

static char * _pfile(int pid, char *file) /* {{{ */
{
	static char tmp[32];
	sprintf(tmp, "/proc/%d/%s", pid, file);
	return tmp;
}
/* }}} */
char * _trim(char *str) /* {{{ */
{
	char *end;
	while (isspace(*str)) str++;
	if (*str == '\0') return str;

	end = str + strlen(str) - 1;
	while (end > str && isspace(*end)) end--;

	*(end + 1) = 0;
	return str;
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
		if (pids[i] == 2 || pids[i] == 1) continue;
		FILE *io = malloc(sizeof(FILE));
		if (!(io = fopen(_pfile(pids[i], "status"), "r"))) {
			fprintf(stderr, "failed to open stat file for pid %d; %s\n", pids[i], strerror(errno));
			exit(1);
		}
		process_ *process = malloc(sizeof(process_));
		process->stats = malloc(sizeof(stats_));
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
			else if (streq(k, "PPid"))                        process->ppid  = x;
			else if (streq(k, "Threads"))                     process->stats->threads = x;
			else if (streq(k, "State"))                       process->stats->state = x;
			else if (streq(k, "VmPeak"))                      process->stats->vmpeak = x;
			else if (streq(k, "VmSize"))                      process->stats->vmsize = x;
			//else if (streq(k, "VmLck"))                       process->stats-> = x;
			//else if (streq(k, "VmPin"))                       process->stats-> = x;
			else if (streq(k, "VmHWM"))                       process->stats->vmhwm = x;
			else if (streq(k, "VmRss"))                       process->stats->vmrss = x;
			/*else if (streq(k, "RssAnon"))                     process->stats-> = x;
			else if (streq(k, "RssFile"))                     process->stats-> = x;
			else if (streq(k, "RssShmem"))                    process->stats-> = x;
			else if (streq(k, "VmData"))                      process->stats-> = x;
			else if (streq(k, "VmStk"))                       process->stats-> = x;
			else if (streq(k, "VmExe"))                       process->stats-> = x;
			else if (streq(k, "VmLib"))                       process->stats-> = x;
			else if (streq(k, "VmPTE"))                       process->stats-> = x;
			else if (streq(k, "VmPMD"))                       process->stats-> = x;
			else if (streq(k, "VmSwap"))                      process->stats-> = x;
			*/
			else if (streq(k, "voluntary_ctxt_switches"))     process->stats->vctx = x;
			else if (streq(k, "nonvoluntary_ctxt_switches"))  process->stats->ictx = x;
		}
		fclose(io);
		if (process->ppid == 2) continue;

		if (!(io = fopen(_pfile(pids[i], "cmdline"), "r"))) {
			fprintf(stderr, "failed to open cmdline file for pid %d; %s\n", process->pid, strerror(errno));
			exit(1);
		}
		size_t len = 0;
		char *path;
		if (getline(&path, &len, io) == -1) {
			fclose(io);
			fprintf(stderr, "failed to read cmdline file for pid %d; %s\n", process->pid, strerror(errno));
			exit(1);
		}
		fclose(io);
		for (unsigned int j = 0; j < len - 1; j++)
			if (path[j] == '\0') path[j] = ' ';
		path[len - 1] = '\0';
		process->cmd = _trim(path);
		processes[current++]  = process;
	}
	return current;
}
/* }}} */
