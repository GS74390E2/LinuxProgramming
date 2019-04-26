#include <unistd.h>
#include <stdio.h>
#include "../lib/tlpi_hdr.h"

extern char **environ;

int main(int argc, char *argv[]){
	char **ep;
	pid_t pid, ppid;

	for(ep = environ; *ep != NULL; ++ep)
		puts(*ep);

	pid = getpid();
	printf("\npid: %d\n", pid);
	ppid = getppid();
	printf("ppid: %d\n", ppid);
	exit(EXIT_SUCCESS);
}
