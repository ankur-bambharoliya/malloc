#ifndef MALLOC_LIB_H
#define MALLOC_LIB_H
#endif

#include "data.h"

//lock
extern __thread Arena *arena;
extern Arena *mainArena;
extern int num_of_arenas;
extern int hblks;     /* Number of mmapped regions */
extern int hblkhd;    /* Space allocated in mmapped regions (bytes) */
extern pthread_mutex_t lock;


void splitRegion(void* hdr_ptr, size_t size);
//increase the program break to accomodate the given size memory request.

void *allocateHeap(size_t size);

//returns the free segments with size greater or equal to given size;
MallocHeader *findFreeSegment(size_t size);

//calculate segment size based on alignment
long long getAlignedSize(size_t alignment, size_t size);

//get the data pointer based on alignment.
void *getDataPtr(MallocHeader* hdr, size_t alignment);

MallocHeader *findSegment(void *ptr);

MallocHeader *getLastSegment();
//A->B->C
// merge given segment to next segment.
void mergeSegment(MallocHeader *hdr);

size_t getPadding(MallocHeader *hdr);

void *reallocarray(void *ptr, size_t nmemb, size_t size);

void init_arena();

size_t alignToPageSize(size_t size);

