/*
 * 使用fcntl() 和 close() 实现dup() dup2()
 */
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include "../lib/tlpi_hdr.h"
#include <stdio.h>


int my_dup(int fd, const char *file){
	int new_fd, accessMode, flags;
	if(fd == -1)
		errExit("open");

	flags = fcntl(fd, F_GETFL);
	if(flags == -1)
		errExit("fcntl");
	accessMode = flags & O_ACCMODE;
	if(accessMode == O_RDONLY)
		printf("file is readable\n");
	else
		printf("file is writable\n");

	new_fd = open(file, flags);
	// new_fd = dup(fd);
	if(new_fd == -1)
		errExit("open");
	return new_fd;
}

int my_dup2(int fd, int new_fd, const char *file){
	int accessMode, flags;
	if(fd == -1){
		return -1;
	}

	flags = fcntl(fd, F_GETFL);
	if(flags == -1)
		errExit("fcntl");
	accessMode = flags & O_ACCMODE;
	if(accessMode == O_RDONLY)
		printf("file is readable\n");
	else
		printf("file is writable\n");

	close(new_fd);
	new_fd = open(file, flags);
	// new_fd = dup(fd);
	if(new_fd == -1)
		errExit("open");
	return 1;
}

int main(int argc, char *argv[]){
	int fd, new_fd;

	printf("input new fd: ");
	scanf("%d", &new_fd);

	if(argc < 1 || strcmp(argv[1], "--help") == 0)
		usageErr("%s file\n", argv[0]);

	fd = open(argv[1], O_RDONLY);
	// new_fd = my_dup(fd, argv[1]);
	// new_fd = dup(fd);
	// if(dup2(fd, new_fd) == -1)
	if(my_dup2(fd, new_fd, argv[1]) == -1){
		errno = EBADF;
		errExit("dup2");
	}
	printf("fd: %d, new_fd: %d\n", fd, new_fd);
	exit(EXIT_SUCCESS);
}
