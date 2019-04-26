/*
 * 获取和日历时间转换
 */
#include <locale.h>
#include <time.h>	// c标准时间库
#include <sys/time.h>	// 系统时间 
#include "../lib/tlpi_hdr.h"
#define SECONDS_IN_TROPICAL_TEAR (365.24219 * 24 * 60 * 60)

int main(int argc, char *argv[]){
	time_t t;	// 系统时间戳
	struct timeval tv;	// 系统时间结构体
	struct tm *gmp, *locp;
	struct tm gm, loc;

	t = time(NULL);
	printf("Second since the Epoch(1 Jan 1970): %ld\n", (long)t);
	printf("(about %6.3f years)\n", t / SECONDS_IN_TROPICAL_TEAR);

	if(gettimeofday(&tv, NULL) == -1)
		errExit("gettimeofday");
	printf("	gettimeofday() returned %ld secs, %ld microsecs\n", (long)tv.tv_sec, (long)tv.tv_usec);

	gmp = gmtime(&t);	// 将时间按格式写入tm结构体
	if(gmp == NULL)
		errExit("gmtime");
	gm = *gmp;	// 保存结果，否则tm结构体会被其他调用改变
	printf("Broken down by gmtime():\n");
	printf("	year=%d mon=%d mday=%d hour=%d min=%d sec=%d ", gm.tm_year, gm.tm_mon, gm.tm_mday, gm.tm_hour, gm.tm_min, gm.tm_sec);
	printf("wday=%d yday=%d isdst=%d\n", gm.tm_wday, gm.tm_yday, gm.tm_isdst);

	locp = localtime(&t);
	if(locp == NULL)
		errExit("localtime");
	loc = *locp;
	printf("Broken down by localtime():\n");
	printf("	year=%d mon=%d mday=%d hour=%d min=%d sec=%d ", loc.tm_year, loc.tm_mon, loc.tm_mday, loc.tm_hour, loc.tm_min, loc.tm_sec);
	printf("wday=%d yday=%d isdst=%d\n", loc.tm_wday, loc.tm_yday, loc.tm_isdst);

	printf("asctime() formats the gmtime() value as: %s\n", asctime(&gm));
	printf(" ctime() formats the time() value as: %s\n", ctime(&t));

	printf("mktime() of gmtime() value:		%ld secs\n", (long)mktime(&gm));
	printf("mktime() of localtime value:	%ld secs\n", (long)mktime(&loc));
	exit(EXIT_SUCCESS);
}
