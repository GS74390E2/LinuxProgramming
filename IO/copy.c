/*
 * 通用IO模型的四个主要系统调用 open()、read()、write()、close() 函数原型见man2手册
 */
#include <sys/stat.h>
#include <fcntl.h>
#include "../lib/tlpi_hdr.h"

#ifndef BUF_SIZE
#define BUF_SIZE 1024
#endif

int main(int argc, char *argv[]){
	// 文件描述符
	int inputFd, outputFd, openFlags;
	mode_t filePerms;
	ssize_t numRead;
	char buf[BUF_SIZE];

	if(argc != 3 || strcmp(argv[1], "--help") == 0)
		usageErr("%s old-file new-file\n", argv[0]);

	inputFd = open(argv[1], O_RDONLY);
	if(inputFd == -1)
		errExit("opening file %s", argv[1]);
	
	// 打开方式 若不存在则创建、只写、截断为0
	openFlags = O_CREAT | O_WRONLY | O_TRUNC;
	// 权限 所有者rw- 用户组rw- 其他r-
	filePerms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH;
	outputFd = open(argv[2], openFlags, filePerms);
	if(outputFd == -1)
		errExit("opening file %s", argv[2]);

	while((numRead = read(inputFd, buf, BUF_SIZE)) > 0)
		if(write(outputFd, buf, BUF_SIZE) != numRead)
			fatal("couldn't write whole buffer");
	if(numRead == -1)
		errExit("read");

	if(close(inputFd) == -1)
		errExit("close input");
	if(close(outputFd) == -1)
		errExit("close output");

	exit(EXIT_SUCCESS);
}
