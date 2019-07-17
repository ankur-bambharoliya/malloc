#include "data.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <errno.h>
#include<pthread.h>

void __attribute__((constructor)) init_arena_struct();

int i =0;
//header of the first fragment;
MallocHeader *head = NULL;
//lock
pthread_mutex_t lock;
int hblks;     /* Number of mmapped regions */
int hblkhd;    /* Space allocated in mmapped regions (bytes) */

__thread Arena *arena;
//Split the given region into given size used segment and unused segment.
//here size is without header and with padding
#define NUM_CORES sysconf(_SC_NPROCESSORS_ONLN)
int num_of_arenas = 0;
Arena* mainArena = NULL;
int last_used = 0;

size_t alignedToBuddyBoundary(size_t size){
	size_t buddySize = 2;
	while(size>buddySize){
		buddySize *=2;
	}
	return buddySize;
}

void splitRegion(void* hdr_ptr, size_t size){
	//region can't be splitted
	MallocHeader *hdr = hdr_ptr;
	size = alignedToBuddyBoundary(size + sizeof(MallocHeader));
	if(hdr->size < size + sizeof(MallocHeader)){
		return;
	}

	//create header for unused segment
	MallocHeader *unused = hdr_ptr + size;
	unused->size = hdr->size - size;
	unused->next = hdr->next;
	unused->prev = hdr;
	unused->free = FREE;
	unused->mmapped = 0;
	
	MallocHeader *next = hdr->next;
	if(next){
		next->prev = unused;
	}
/*
	char buf[1024];
	snprintf(buf, 1024, "split %p %p %p %p\n", hdr->prev, hdr, unused, hdr->next);
	write(STDOUT_FILENO, buf, strlen(buf) + 1);
*/
	// settting the size of the region to given size
	hdr->size -= unused->size;
	hdr->next = unused;
	++arena->free_blocks;
	++arena->total_blocks;
}

size_t alignToPageSize(size_t size){

	size_t pageSize = sysconf(_SC_PAGESIZE);
	//convert the size into multiple of page size.
	return ((size-1)/pageSize + 1)*pageSize;
	
}

//increase the program break to accomodate the given size memormmapy request.
void *allocateHeap(size_t size){

	size_t allocSize = 4096; //2MB //size + sizeof(MallocHeader); 
	// convert the size into multiple of page size.
	size_t requestSize = alignToPageSize(allocSize);

	//increse program break limit.
	//void *ptr = sbrk(requestSize);
	void *ptr = mmap(NULL, requestSize,  PROT_READ|PROT_WRITE|PROT_EXEC, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);

	//if error then set errno ENOMEM and return NULL. 
	if(errno == ENOMEM)
	{
		errno = ENOMEM;
		return NULL;
	}

  MallocHeader *hdr = (MallocHeader*) ptr;
  hdr->size = requestSize;
	hdr->next = NULL; 	
	hdr->prev = NULL;
	hdr->free = FREE;
	hdr->mmapped = 0;
	//split the allocated segment into used and unused segment.
	splitRegion(hdr, size);
	++arena->total_blocks;
	++arena->free_blocks;
	arena->arena += requestSize;
	arena->arena_space += requestSize; 
	return hdr;
}

//returns the free segments with size greater or equal to given size;
MallocHeader *findFreeSegment(size_t size){
	size += sizeof(MallocHeader);
	MallocHeader *cur=arena->head;
  while (cur && !(cur->free && cur->size >= size )) {
  	cur = cur->next;
	}
	return cur;
}

//returns last segment.
MallocHeader *getLastSegment(){
	MallocHeader *cur=arena->head;
  while (cur->next) {
		cur = cur->next;
	}
	return cur;
}

//calculate segment size based on alignment
long long getAlignedSize(size_t alignment, size_t size){
	if(alignment<8){
		alignment =8;
	}
	size += alignment;
	return size;
}

//set the data pointer based on alignment.
void *getDataPtr(MallocHeader* hdr, size_t alignment){

	void * data_ptr = hdr + 1;
	long long  l_ptr = (long long)(data_ptr-1);
	size_t diff = l_ptr%alignment;
	data_ptr = (void *)(l_ptr + alignment-diff);

	return data_ptr;
}

//find the segment for the given pointer.
MallocHeader *findSegment(void *ptr){
/*
	MallocHeader *cur = arena->head;
	while(cur&& cur->data!=ptr){
		cur=cur->next;
	}
*/
//	return cur;
	return ptr - sizeof(MallocHeader);
}

//A->B->C
// merge given segment to next segment.
void mergeSegment(MallocHeader *hdr)
{
	MallocHeader *next = hdr->next; 

	if( hdr && next && hdr->free && next->free && hdr + hdr->size == next){
		//if we merge A and B then next of A will be C.
		hdr->next = next->next;
		hdr->size += next->size;
		
		//if we merge A and B then prev of C will be A.
		next = hdr->next;
		if(next){
			next->prev = hdr;
		}
		--arena->free_blocks;
		--arena->total_blocks;
	}
}

void init_arena_struct(){
	if(mainArena != NULL) return;
	num_of_arenas = sysconf(_SC_NPROCESSORS_ONLN);
	int totalSize = alignToPageSize(num_of_arenas * sizeof(Arena));
	mainArena = sbrk(totalSize);
	Arena *cur;	
	int i =0;
	for(i =0;i<num_of_arenas;i++){
		cur = mainArena + i;
		cur->head = NULL;
		cur->total_blocks = 0;
		cur->used_blocks = 0;
		cur->free_blocks = 0;
		cur->allocation_request = 0;
		cur->free_request=0;
		cur->arena_space=0;
		cur->arena=0;
	}
}

void init_arena(){
			int i = last_used;
			++last_used;
			arena = mainArena + (i%num_of_arenas);
}
