#define _GNU_SOURCE
#include <stdlib.h>
#include "../lib/tlpi_hdr.h"

extern char **environ;

int my_set(const char *name, const char *value, int overwrite){
	char *old_value = getenv(name), *ret;
	char *new_name = (char *)malloc(strlen(name) + 2);
	strcpy(new_name, name);
	strcat(new_name, "=");

	if(old_value != NULL && overwrite == 0){
		ret = (char *)malloc(strlen(name) + strlen(old_value) + 2);
		strcpy(ret, new_name);
		strcat(ret, old_value);
	}
	else{
		ret = (char *)malloc(strlen(name) + strlen(value) + 2);
		strcpy(ret, new_name);
		strcat(new_name, value);
	}
	if(putenv(ret) != 0)
		errExit("putenv: %s", ret);
	return 0;
}

int my_unset(const char *name){
	char **ep;
	char *value = getenv(name);
	char *new_name = (char *)malloc(strlen(name) + strlen(value) + 2);
	strcpy(new_name, name);
	strcat(new_name, "=");
	strcat(new_name, value);
	
	for(ep = environ; *ep != NULL; ++ep){
		if(strcmp(*ep, new_name) == 0){
			*ep = NULL;
		}
	}
	return 0;
}

int main(int argc, char *argv[]){
	int j;
	char **ep;

	clearenv();

	for(j = 1; j < argc; ++j)
		if(putenv(argv[j]) != 0)
			errExit("putenv: %s", argv[j]);

	if(my_set("GREET", "Hello world", 0) == -1)
		errExit("setenv");

	my_unset("BYE");

	for(ep = environ; *ep != NULL; ++ep)
		puts(*ep);

	exit(EXIT_SUCCESS);
}
