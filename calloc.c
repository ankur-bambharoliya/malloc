#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <errno.h>
#include<pthread.h>

void *calloc(size_t nmemb, size_t size){

	size_t totalSize = nmemb*size;
	void *ptr = malloc(totalSize);
	if(ptr!=NULL){
		memset(ptr, 0, totalSize);
		if(errno){
		  char buf[1024];
			snprintf(buf, 1024, "Error occured in memset::%d\n",errno);
			write(STDOUT_FILENO, buf, strlen(buf) + 1);
			return NULL;
		}
	} 
	return ptr;
}

