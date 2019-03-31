/*
 * 常用异常处理函数头文件
 */
#ifndef __TLPI_HDR_H
#define __TLPI_HDR_H

#include <sys/types.h>			// 现代类unix程序不必包含此头文件，添加是为程序的可移植性，Linux平台无必要
#include <stdio.h>				// 标准io库
#include <stdlib.h>				// 包含标准系统库函数原型
#include <unistd.h>				// 许多系统call的原型
#include <errno.h>				// 包含errno声明，定义了error的一些常量
#include <string.h>				// 常用字符串处理函数

#include "get_num.h"			// 声明了自定义处理数值型参数的方法
#include "error_functions.h"	// 声明了自定义错误处理方法

typedef enum {FALSE, TRUE} Boolean;

#define min(m, n) ((m) < (n) ? (m) : (n))
#define max(m, n) ((m) > (n) ? (m) : (n))

#endif
