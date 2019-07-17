#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <malloc.h>

int main(int argc, char **argv)
{
  size_t size = 12;

  void *mem = memalign(64, 500);
  void *mem2 = calloc(500, 12);
	char buf[1024];

	snprintf(buf, 1024, "Successfully malloc'd %zu bytes at addr %p\n", size, mem);
	write(STDOUT_FILENO, buf, strlen(buf) + 1);

  free(mem);
	snprintf(buf, 1024, "Successfully free'd %zu bytes from addr %p\n", size, mem);
	write(STDOUT_FILENO, buf, strlen(buf) + 1);

	snprintf(buf, 1024, "Successfully free'd %zu bytes from addr %p\n", size, mem2);
	write(STDOUT_FILENO, buf, strlen(buf) + 1);

  return 0;
}
