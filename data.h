#ifndef DATA_H
#define DATA_H
#endif

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <errno.h>
#include<pthread.h>

#define FREE 1
#define OCCUPIED 0

typedef struct MallocHeader
{
  size_t size; //size of header + data
	short free;
	void *next;
	void *prev;
	short mmapped;
} MallocHeader;


typedef struct Arena
{
	pthread_mutex_t lock; // arena specific lock
	MallocHeader *head;		// pointer to first block of arena
	int total_blocks;			// total number of blocks
	int used_blocks;			// used blocks
	int free_blocks;			// free blocks
	int allocation_request;	// number of malloc request to arena
	int free_request;			// number of free request to arena
	int arena_space;		// total space allocated to arena
 	int arena;     /* Non-mmapped space allocated (bytes)*/
 	int uordblks;  /* Total allocated space (bytes) */
} Arena;

