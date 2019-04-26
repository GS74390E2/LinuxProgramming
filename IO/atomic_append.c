#define _FILE_OFFSET_BITS 64
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include "../lib/tlpi_hdr.h"

int main(int argc, char *argv[]){
	int fd, openFlag;
	ssize_t numWritten;
	
	if(argc < 3 || strcmp(argv[1], "--help") == 0)
		usageErr("%s file num-bytes [x]\n", argv[1]);

	openFlag = O_WRONLY | O_CREAT;
	fd = open(argv[1], openFlag, S_IRUSR | S_IWUSR);
	if(argc == 3){
		openFlag = fcntl(fd, F_GETFL);
		if(openFlag == -1)
			errExit("fcntl");
		openFlag |= O_APPEND;
		if(fcntl(fd, F_SETFL, openFlag) == -1)
			errExit("fcntl");
	}
	if((openFlag & O_APPEND) && argc == 3){
		printf("append mode %d\n", openFlag);
		numWritten = getLong(&argv[2][0], GN_ANY_BASE, argv[2]);
		while(numWritten > 0){
			write(fd, "g", 1);
			--numWritten;
		}
	}
	else if(!(openFlag & O_APPEND) && argc == 4 && strcmp(argv[3], "x") == 0){
		printf("lseek mode %d\n", openFlag);
		numWritten = getLong(&argv[2][0], GN_ANY_BASE, argv[2]);
		while(numWritten > 0){
			lseek(fd, 0, SEEK_END);
			write(fd, "g", 1);
			--numWritten;
		}
	}
	else
		errExit("wrong");
	exit(EXIT_SUCCESS);
}
