#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <sys/stat.h>
#include <stdio.h>

#define MAX_PIDS 1024

static char * _pfile(int pid, char *file)
{
	static char tmp[32];
	sprintf(tmp, "/proc/%d/%s", pid, file);
	return tmp;
}

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

/*
process_ pull_stats(int pid)
{
	process_ proc;
	return proc;
}
*/

int main (void)
{
	pid_t pids[MAX_PIDS];
	DIR *dp;
	struct dirent *ep;

	int count = 0;
	dp = opendir ("/proc");
	if (dp != NULL) {
		while ((ep = readdir (dp)))
			if (strspn(ep->d_name, "0123456789") == strlen(ep->d_name))
				pids[count++] = atoi(ep->d_name);
		(void) closedir (dp);
	} else
		perror("Couldn't open /proc dir\n");

	for (int i = 0; i < count; i++) {
		if (pids[i] == 2 || pids[i] == 1) continue;
		FILE *io = malloc(sizeof(FILE));
		if (!(io = fopen(_pfile(pids[i], "stat"), "r"))) {
			fprintf(stderr, "failed to open stat file for pid %d; %s\n", pids[i], strerror(errno));
			exit(1);
		}
		int ppid;
		int pid;
		char *tcomm = malloc(128);
		char state;
		fscanf(io, "%d %s %c %d ", &pid, tcomm, &state, &ppid);
		fclose(io);
		if (ppid == 2) continue;
		process_ process;
		process.pid  = pid;
		process.ppid = ppid;
		if (!(io = fopen(_pfile(pids[i], "cmdline"), "r"))) {
			fprintf(stderr, "failed to open cmdline file for pid %d; %s\n", pids[i], strerror(errno));
			exit(1);
		}
		size_t len = 0;
		char *path;
		getline(&path, &len, io);
		fclose(io);
		for (unsigned int j = 0; j < len - 1; j++)
			if (path[j] == '\0')
				path[j] = ' ';
		path[len - 1] = '\0';
		process.cmd = path;
		printf("pid [%d] cmd [%s]\n", process.pid, process.cmd);

	}

	return 0;
}
