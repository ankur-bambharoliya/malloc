#include "malloc_lib.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <errno.h>
#include<pthread.h>

void free(void* ptr){
	if(ptr == NULL||arena==NULL){
		return; 
	}
	
	MallocHeader *hdr= findSegment(ptr);
	if(hdr!=NULL){
	++arena->free_request;

	if(hdr->mmapped && !hdr->free ){
		pthread_mutex_lock(&lock);
		--hblks;
		hblkhd -= hdr->size;
		pthread_mutex_unlock(&lock);
		hdr->free = FREE;
		munmap(hdr, hdr->size);
		return;			
	}
	hdr->free = FREE;

	pthread_mutex_lock(&arena->lock);
	--arena->used_blocks;
	++arena->free_blocks;
	arena->uordblks -= hdr->size;
	MallocHeader *prev = hdr->prev;
	//merge previous segment if its free
 if(hdr->prev!=NULL&& prev->free){
				mergeSegment(prev);
				hdr = prev;
	}

	//merge next segment if its free
	MallocHeader *next = hdr->next;
	if(next!=NULL&& next->free){
			mergeSegment(hdr);
	}

	//if the last node;
	if(!hdr->next){
				prev = hdr->prev;
				if(prev){
					//second last node becomes last node.
					prev->next = NULL; 
				}else{ // if no second last node then there is only one node.
					arena->head = NULL;
				}
		//munmap(hdr, hdr->size);
		//brk(hdr);
	}
	pthread_mutex_unlock(&arena->lock);
	}
}

