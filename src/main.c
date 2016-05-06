#include "proccer.h"


int main (void)
{
	process_ **processes = malloc(sizeof(process_ *) * 1024);
	int size;
	while (1) {
		sleep(1);
		if ((size = get_procs(processes)) == 0)
			exit(1);
		for (int i = 0; i < size; i++)
			printf("pid [ %u ] ppid [ %u ] states [ %c ] procs [ %u ] cmd [ %s ]\n", processes[i]->pid, processes[i]->ppid, processes[i]->stats->state, processes[i]->stats->procs, processes[i]->cmd);
	}
	//process_ ***procs = &processes;
	//for (int i = 0; i < size; i++)
	//	printf("pid [ %u ] threads [ %lu ] VCtx [ %lu ] cmd [ %s ]\n", (*procs)[i]->pid, (*procs)[i]->stats->threads, (*procs)[i]->stats->vctx, (*procs)[i]->cmd);
}
