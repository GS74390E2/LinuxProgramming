#define _GNU_SOURCE		// stdlib的一种不同的实现
#include <stdlib.h>
#include "../lib/tlpi_hdr.h"

extern char **environ;

int main(int argc, char *argv[]){
	int j;
	char **ep;

	clearenv();		// 清空环境变量

	for(j = 1; j < argc; ++j)	// 使用命令行参数设置环境变量 =方式
		if(putenv(argv[j]) != 0)
			errExit("putenv: %s", argv[j]);

	if(setenv("GREET", "Hello world", 0) == -1)		// 设置环境变量
		errExit("setenv");

	unsetenv("BYE");	// 删除环境变量BYE

	for(ep = environ; *ep != NULL; ++ep)
		puts(*ep);

	exit(EXIT_SUCCESS);
}

