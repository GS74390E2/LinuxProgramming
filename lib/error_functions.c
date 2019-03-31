#include <stdarg.h>					// 提供对可变参数处理的函数
#include "error_functions.h"
#include "tlpi_hdr.h"
#include "ename.c.inc"				// 定义ename 和 MAX_ENAME

#ifdef __GNUC__
__attribute__ ((__noreturn__))
#endif

static void
terminate(Boolean useExit3){
	char *s;
	/*
	 * 当EF_DUMPCORE 环境变量被定义并且非空；
	 * call exit(3) or _exit(2), 取决于参数useExit3
	 */
	s = getenv("EF_DUMPCORE");
	if(s != NULL && *s != '\0')
		// 非阻塞型终止程序的信号,若程序caught这个信号则不会终止
		abort();
	else if(useExit3)
		exit(EXIT_FAILURE);
	else
		_exit(EXIT_FAILURE);
}

static void
outputError(Boolean useErr, int err, Boolean flushStdout, const char *format, va_list ap){
#define BUF_SIZE 500
	char buf[BUF_SIZE], userMsg[BUF_SIZE], errText[BUF_SIZE];

	// 基本等同于snprintf，自动调用va_arg()宏，ap参数代表可变参数,将格式化字符串与后面的字符串的一一对应转换为ap可变参数变量
	vsnprintf(userMsg, BUF_SIZE, format, ap);

	if(useErr)
		// snprintf：往arg1代表的字符串中写入arg3代表的字符串，用于most size bytes输出，包含'\0'
		// 若第三个为格式化字符串，在后面增加对应的字符串
		snprintf(errText, BUF_SIZE, " [%s %s]", (err > 0 && err < MAX_ENAME) ? ename[err]: "?UNKNOWN?", strerror(err));
	else
		snprintf(errText, BUF_SIZE, ":");

	snprintf(buf, BUF_SIZE, "ERROR%s %s\n", errText, userMsg);

	if(flushStdout)
		fflush(stdout);		// 刷新所有等待在标准输出的消息
	fputs(buf, stderr);		// 向标准错误写入buf，结尾不带结束字符串'\0'
	fflush(stderr);			// stderr非行缓冲
}

void
errExit(const char *format, ...){
	/*
	 * va_list 是stdarg定义的一个数据结构，需要使用对应的方法来进行处理
	 * va_start(arg1, arg2) arg1：va_list型变量，arg2：可变参数前最后一个参数（其类型必须被声明）
	 * va_arg(arg, type) arg：在start中被初始化的变量，type为对应类型，若类型匹配会返回剩余参数
	 * va_end(arg) arg：在va_start中被初始化的va_list变量，和start一一对应，undefine va_list
	 */
	va_list argList;
	va_start(argList, format);
	outputError(TRUE, errno, TRUE, format, argList);
	va_end(argList);

	terminate(TRUE);
}

void
fatal(const char *format, ...){
	va_list argList;
	va_start(argList, format);
	outputError(FALSE, 0, TRUE, format, argList);
	va_end(argList);
	terminate(TRUE);
}

void
usageErr(const char *format, ...){
	va_list argList;
	fflush(stdout);

	fprintf(stderr, "Usage: ");
	va_start(argList, format);
	vfprintf(stderr, format, argList);
	va_end(argList);

	fflush(stderr);
	exit(EXIT_FAILURE);
}
