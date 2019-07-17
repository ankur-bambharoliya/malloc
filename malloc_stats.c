#include "malloc_lib.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <errno.h>
#include<pthread.h>
#include<malloc.h>

void malloc_stats(){

	int n_arena = 0;
	int total_arena_space = 0;
	Arena *cur;
	int i=0;
	for(i=0;i<num_of_arenas;i++){
		cur = mainArena + i;
		if(cur->head){
			n_arena +=0;
			total_arena_space +=cur->arena_space;			
		}
	}

	char buf[1024];
	// space allocated to arena. This space doesn't include the mmapped space.
	snprintf(buf, 1024, "Total size of arena allocated with sbrk/mmap: %d\n", total_arena_space);
	write(STDOUT_FILENO, buf, strlen(buf) + 1);

	snprintf(buf, 1024, "Total number of arenas: %d\n", n_arena);
	write(STDOUT_FILENO, buf, strlen(buf) + 1);	

	for(i=0;i<num_of_arenas;i++){
		if(!cur->head)continue;

		cur = mainArena + i;
		snprintf(buf, 1024, "\nArena::%d\n", i);
		write(STDOUT_FILENO, buf, strlen(buf) + 1);	

		snprintf(buf, 1024, "Total number of blocks: %d\n", cur->total_blocks);
		write(STDOUT_FILENO, buf, strlen(buf) + 1);	

		snprintf(buf, 1024, "Used blocks: %d\n", cur->used_blocks);
		write(STDOUT_FILENO, buf, strlen(buf) + 1);	

		snprintf(buf, 1024, "Free blocks:	%d\n", cur->free_blocks);
		write(STDOUT_FILENO, buf, strlen(buf) + 1);	

		snprintf(buf, 1024, "Total allocation requests: %d\n", cur->allocation_request);
		write(STDOUT_FILENO, buf, strlen(buf) + 1);	

		snprintf(buf, 1024, "Total free requests: %d\n", cur->free_request);
		write(STDOUT_FILENO, buf, strlen(buf) + 1);	
	}

}


struct mallinfo mallinfo(void){
	struct mallinfo m;
	m.arena = 0;
 	m.ordblks =0;   /* Number of free chunks */
 	m.hblks = hblks;     /* Number of mmapped regions */
 	m.hblkhd = hblkhd;    /* Space allocated in mmapped regions (bytes) */
 	m.uordblks = 0;  /* Total allocated space (bytes) */

	Arena *cur;
	int i=0;
	for(i=0;i<num_of_arenas;i++){
		cur = mainArena + i;
		m.arena += cur->arena;
		m.ordblks += cur->free_blocks;
		m.uordblks += cur->uordblks;
	}
	m.fordblks = m.arena - m.uordblks;

	return m;
}	
