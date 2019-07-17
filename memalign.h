#ifndef MEMALIGN_H
#define MEMALIGN_H
#endif

void *memalign(size_t alignment, size_t size);
int posix_memalign(void **memptr, size_t alignment, size_t size);
