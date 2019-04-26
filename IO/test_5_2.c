#define _FILE_OFFSET_BITS 64
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "../lib/tlpi_hdr.h"

int main(int argc, char *argv[]){
	int fd;
	ssize_t numWritten;

	if(argc != 2 || strcmp(argv[1], "--help") == 0)
		usageErr("%s file\n", argv[0]);

	fd = open(argv[1], O_WRONLY | O_APPEND, S_IRUSR | S_IWUSR);
	if(fd == -1)
		errExit("open");

	// 从当前位置写入
	// numWritten = pwrite(fd, "test", 4, 0);
	lseek(fd, 0, SEEK_SET);
	numWritten = write(fd, "test", 4);
	if(numWritten == -1)
		errExit("write");

	if(close(fd) == -1)
		errExit("close");

	exit(EXIT_SUCCESS);
}
