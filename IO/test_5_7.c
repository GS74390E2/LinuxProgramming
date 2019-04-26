/*
 * 使用read() write() malloc 实现readv() writev()
 */
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/uio.h>
#include "../lib/tlpi_hdr.h"


int my_readv(int fd, struct iovec *iov, int num);

int my_writev(int fd, struct iovec *iov, int num);

int main(int argc, char *argv[]){
	int fd;
	struct iovec iov[3];
	struct stat myStruct;	// first buffer
	int x;					// second buffer
#define STR_SIZE 100
	char str[STR_SIZE];		// third buffer
	ssize_t numRead, totRequired, numWritten;

	if(argc < 2 || strcmp(argv[1], "--help") == 0)
		usageErr("%s file\n", argv[0]);

	fd = open(argv[1], O_RDONLY);
	if(fd == -1)
		errExit("open");

	totRequired = 0;

	iov[0].iov_base = &myStruct;
	iov[0].iov_len = sizeof(myStruct);
	totRequired += iov[0].iov_len;

	iov[1].iov_base = &x;
	iov[1].iov_len = sizeof(x);
	totRequired += iov[1].iov_len;

	iov[2].iov_base = str;
	iov[2].iov_len = sizeof(str);
	totRequired += iov[2].iov_len;

	numRead = my_readv(fd, iov, 3);
	// numRead = readv(fd, iov, 3);
	if(numRead == -1)
		errExit("readv");

	if(numRead < totRequired)
		printf("Read fewer bytes than requested\n");

	int new_fd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
	
	if(new_fd == -1)
		errExit("open");

	numWritten = my_writev(new_fd, iov, 3);
	// numWritten = writev(new_fd, iov, 3);
	if(numWritten == -1)
		errExit("writev");

	if(numWritten < totRequired)
		printf("write fewer bytes than requested\n");

	printf("total bytes requested: %ld; bytes read: %ld\n", (long)totRequired, (long)numRead);
	printf("total bytes requested: %ld; bytes write: %ld\n", (long)totRequired, (long)numWritten);
	exit(EXIT_SUCCESS);
}

int my_readv(int fd, struct iovec *iov, int num){
	int i;
	ssize_t numRead = 0;
	for(i = 0; i != num; ++i){
		numRead += read(fd, iov[i].iov_base, iov[i].iov_len);
		if(numRead == -1)
			errExit("read");
	}
	return (int)numRead;
}

int my_writev(int fd, struct iovec *iov, int num){
	ssize_t numWritten = 0;
	int i;

	for(i = 0; i != num; ++i){
		numWritten += write(fd, iov[i].iov_base, iov[i].iov_len);
		if(numWritten == -1)
			errExit("write");
	}
	return (int)numWritten;
}

