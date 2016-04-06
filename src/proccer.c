#include "proccer.h"

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
	if (*str == 0) return str;

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
	for (int i = 0; i < count; i++) {
		if (pids[i] == 2 || pids[i] == 1) continue;
		FILE *io = malloc(sizeof(FILE));
		if (!(io = fopen(_pfile(pids[i], "stat"), "r"))) {
			fprintf(stderr, "failed to open stat file for pid %d; %s\n", pids[i], strerror(errno));
			exit(1);
		}
		char *tcomm = malloc(128);
		char state;
		process_ *process = malloc(sizeof(process_));
		// memset(process, 0, sizeof(process_));
		if (fscanf(io, "%u %s %c %u ", &process->pid, tcomm, &state, &process->ppid) == 0) {
			fprintf(stderr, "failed to read stat file for pid %u: %s\n", process->pid, strerror(errno));
			exit(1);
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
			if (path[j] == '\0')
				path[j] = ' ';
		path[len - 1] = 0;
		process->cmd = _trim(path);
		processes[current++]  = process;
	}
	return current;
}
/* }}} */
