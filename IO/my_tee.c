/*
 * tee 从标准输入读取数据至结尾，将数据写入标准输出和命令行参数指定文件
 * 会覆盖同名文件
 * -a 参数能够追加写入
 */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "../lib/tlpi_hdr.h"

#ifndef BUF_SIZE
#define BUF_SIZE 1024
#endif

typedef enum {False, True} bool;

int main(int argc, char *argv[]){
	int c, fd, openFlags;
	mode_t filePerms;
	ssize_t numRead;
	char buf[BUF_SIZE];
	bool write_flag = False;

	if(argc > 1 && strcmp(argv[1], "--help") == 0)
		usageErr("%s file {<filename>|-a<filename>}\n", argv[0]);

	while((c = getopt(argc, argv, "a:")) != -1)
		switch(c){
			case 'a':
				printf("%s", optarg);
				openFlags = O_WRONLY | O_APPEND;
				filePerms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP;
				fd = open(optarg, openFlags, filePerms);
				write_flag = True;
				break;
			default:
				abort();
		}

	// 标准输入 => 标准输出
	// 标准输入 => 指定文件、标准输出
	if(!write_flag && argv[1]){
		openFlags = O_WRONLY | O_CREAT | O_TRUNC | O_APPEND;
		filePerms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP;
		fd = open(argv[1], openFlags, filePerms);
	}

	while((numRead = read(STDIN_FILENO, buf, BUF_SIZE)) > 0){
		write(STDOUT_FILENO, buf, numRead);
		if(fd > 0)
			write(fd, buf, numRead);
		}

	if(numRead == -1)
		errExit("read");

	if(fd != -1 && close(fd) == -1)
		errExit("close %s", argv[1]);

	return EXIT_SUCCESS;
}
