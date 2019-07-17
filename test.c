#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

typedef struct MallocHeader
{
  size_t size;
	void* next;
	void* prev;
} MallocHeader;


int main()
{
	int i =5;
	void *ptr = &i;
	long long  ptr1 = ptr-1;
	printf("%p\n", ptr);	
	ptr = (void *)ptr1;
	size_t diff = ptr1%4;
	ptr = (void *)(ptr1 + 4-diff);
	printf("%p, %ld, %d , %d\n", ptr,ptr1,diff, sizeof(long long));
} 
