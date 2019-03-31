/* 
 * 解析数值型命令行参数，如果参数未包含一个有效整数字符串(仅包含数字以及'-' 和 '+')，会打印错误信息终止程序
 */
#ifndef __GET_NUM_H
#define __GET_NUM_H

#define GN_NONNEG 01	// value must be >= 0
#define GN_GT_0 02		// value must be >0

#define GN_ANY_BASE 0100
#define GN_BASE_8 0200
#define GN_BASE_16 0400

long getLong(const char *arg, int flags, const char *name);

int getInt(const char *arg, int flags, const char *name);

#endif
