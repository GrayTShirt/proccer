#include "proccer.h"


int main (void)
{
	process_ **processes = malloc(sizeof(process_ *) * 1024);
	int size;
	if ((size = get_procs(processes)) == 0)
		exit(1);
	process_ ***procs = &processes;
	for (int i = 0; i < size; i++)
		printf("pid [%d] cmd [%s]\n", (*procs)[i]->pid, (*procs)[i]->cmd);
}
