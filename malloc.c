#include "malloc_lib.h"
#include "memalign.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <errno.h>
#include<pthread.h>


void *malloc(size_t size)
{	
		MallocHeader *hdr;
		if(size + sizeof(MallocHeader)>=4096){
			size_t requestSize =  alignToPageSize(size + sizeof(MallocHeader));
			hdr = mmap(NULL, requestSize,  PROT_READ|PROT_WRITE|PROT_EXEC, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
			hdr->free = OCCUPIED;
			hdr->mmapped = 1;
	pthread_mutex_lock(&lock);
			hblks+=1;
			hblkhd += requestSize;
	pthread_mutex_unlock(&lock);
			hdr->size = requestSize;
		}else{
			
			//size_t void_size = sizeof(void*);
			if(size==0){
				return NULL;
			}
			
			//MallocHeader *hdr;
			if(arena == NULL){
				init_arena();
			}
			pthread_mutex_lock(&arena->lock);
			++arena->allocation_request;

			//first allocation. 
			if(arena->head == NULL){
				hdr = allocateHeap(size);
				arena->head = hdr;

			}else{
				hdr = findFreeSegment(size);
				if(hdr == NULL){
					MallocHeader *last = getLastSegment();
					hdr = allocateHeap(size);
					if(hdr == NULL){
						errno = ENOMEM;
						return NULL;
					}
					last->next = hdr;
					hdr->prev = last;
				}else{
					splitRegion(hdr, size);
				}
			}
			--arena->free_blocks;
			++arena->used_blocks;
			arena->uordblks += hdr->size;
			hdr->free = OCCUPIED;
			hdr->mmapped = 0;
			pthread_mutex_unlock(&arena->lock);
	}
	return hdr + 1;
}
