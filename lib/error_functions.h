/*
 * 常用错误处理函数的声明
 */
#ifndef __ERROR_FUNCTIONS_H
#define __ERROR_FUNCTIONS_H

void errMsg(const char *format, ...);

#ifdef __GNUC__
/* 这个宏会停止 gcc -Wall 的警告：控制到达非void函数结尾（有返回值函数并为返回）
 * 因为使用以下函数会因为一些错误终止main()函数or一些非void函数再函数结束之前
 */

#define NORETURN __attribute__ ((__noreturn__))
#else
#define NORETURN
#endif

// 在stderr打印消息，终止程序
void errExit(const char *format, ...) NORETURN;

// 诊断一般性错误，包括未设置errno的库函数错误，其他与errExit相同
void fatal(const char *format, ...) NORETURN;

// 诊断命令行参数使用方面的错误，风格与printf相同，在标准错误打印字符串 Usage: 随之格式化输出，终止程序
void usageErr(const char *format, ...) NORETURN;

#endif
