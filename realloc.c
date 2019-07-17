#include "malloc_lib.h"
#include "free.h"
#include "malloc.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <errno.h>
#include<pthread.h>

void *realloc(void *ptr, size_t size){
/*
	char buf[1024];
	snprintf(buf, 1024, "realloc request %p\n", ptr);
	write(STDOUT_FILENO, buf, strlen(buf) + 1);
*/

	if(ptr == NULL){
		return malloc(size);
	}
	MallocHeader *hdr;
	
	if(arena != NULL){
		hdr = findSegment(ptr);
	}

	void * realloc_ptr = NULL;
	if(hdr||hdr->mmapped){
		size_t oldSize = hdr->size - sizeof(MallocHeader);
		// if size is zero then free memory.		
		if(size==0){
			free(ptr);
			realloc_ptr = NULL;
		}else if(oldSize>=size){ //reducing the size
			pthread_mutex_lock(&arena->lock);
			splitRegion(hdr, size); // size should pass padding
			realloc_ptr = ptr;
			arena->uordblks -= oldSize - hdr->size + sizeof(MallocHeader);
			pthread_mutex_unlock(&arena->lock);			
		}else{// incresing the size
				void* newPtr = malloc(size);
				if(newPtr){
					memcpy(newPtr, ptr, oldSize); // copy content from old segment
				}
				free(ptr);
				realloc_ptr = newPtr;			
		}		
	}	
	return realloc_ptr;
}


void *reallocarray(void *ptr, size_t nmemb, size_t size){
	long long x = size * nmemb;
	if (size != 0 && x / size != nmemb) {
		errno = ENOMEM;
		return NULL;
	}

	return realloc(ptr, x);
}


