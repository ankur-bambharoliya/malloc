#include "malloc_lib.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <errno.h>
#include<pthread.h>

void *memalign(size_t alignment, size_t size)
{	//check power of 2 and multiple of void*

	size_t void_size = sizeof(void*);
	if(alignment % void_size || (alignment)&(alignment-1) || alignment == 1)	
	{
		errno = EINVAL;
		return NULL;
	}
	if(size==0){
		return NULL;
	}
	size = getAlignedSize(alignment, size);

	size_t requestSize =  alignToPageSize(size + sizeof(MallocHeader));
	void *ptr = mmap(NULL, requestSize,  PROT_READ|PROT_WRITE|PROT_EXEC, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
	if(errno == ENOMEM)
	{
		errno = ENOMEM;
		return NULL;
	}

	MallocHeader *hdr = getDataPtr(ptr, alignment) - sizeof(MallocHeader);
	hdr->free = OCCUPIED;
	hdr->mmapped = 1;
	size_t unUsed = ((void *)hdr) - ptr;
	//munmap(ptr, unUsed-1);
	hdr->size = requestSize - unUsed;
	return hdr + 1;
}

int posix_memalign(void **memptr, size_t alignment, size_t size){
	void* ptr = memalign(alignment, size);
	*memptr = ptr;
	if(errno == ENOMEM|| errno == EINVAL){
		return errno;
	}
	return 0;
}

