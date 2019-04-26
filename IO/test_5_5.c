/*
 * 验证文件描述符及其副本是否共享了系统及文件表
 */
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include "../lib/tlpi_hdr.h"

int main(int argc, char *argv[]){
	int fd, new_fd, openFlags, new_openFlags;
	off_t offset, new_offset;

	if(argc < 2 || strcmp(argv[1], "--help") == 0)
		usageErr("%s file\n", argv[0]);

	fd = open(argv[1], O_RDWR, S_IRUSR | S_IWUSR);
	if(fd == -1)
		errExit("open");

	new_fd = dup(fd);
	if(new_fd == -1)
		errExit("dup");
	printf("fd: %d, new_fd: %d\n", fd, new_fd);

	openFlags = fcntl(fd, F_GETFL);
	new_openFlags = fcntl(new_fd, F_GETFL);
	if(openFlags == new_openFlags)
		printf("flag is same.\n");
	
	offset = lseek(fd, 0, SEEK_CUR);
	new_offset = lseek(new_fd, 0, SEEK_CUR);
	if(offset == new_offset)
		printf("offset is same.\n");

	exit(EXIT_SUCCESS);
}

