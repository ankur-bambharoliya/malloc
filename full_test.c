#include <stdio.h>
#include "realloc.h"
#include "malloc.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <errno.h>
#include<pthread.h>

int main(int argc, char **argv){
	char buf1[1024];
	void * start = sbrk(0);
	snprintf(buf1, 1024, "Start::%p\n",start);
  write(STDOUT_FILENO, buf1, strlen(buf1) + 1);
	void *ptr_malloc = malloc(100);	
	snprintf(buf1, 1024, "malloc 100::%p\n",ptr_malloc);
  write(STDOUT_FILENO, buf1, strlen(buf1) + 1);

	void *ptr_malloc2 = malloc(3900);	
	snprintf(buf1, 1024, "malloc 3900::%p\n",ptr_malloc2);
  write(STDOUT_FILENO, buf1, strlen(buf1) + 1);


	void *ptr_malloc3 = malloc(782);	
	snprintf(buf1, 1024, "malloc 782::%p\n",ptr_malloc3);
  write(STDOUT_FILENO, buf1, strlen(buf1) + 1);
	free(ptr_malloc2);
	free(ptr_malloc3);
	free(ptr_malloc);
	void * end = sbrk(0);
	snprintf(buf1, 1024, "end::%p\n",end);
  write(STDOUT_FILENO, buf1, strlen(buf1) + 1);

}
