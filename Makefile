#Sample Makefile for Malloc
CC=gcc
CFLAGS=-g -O0 -fPIC -Werror -Wall

TESTS=t-test1 test1 test2
HEADERS= malloc_lib.h data.h memalign.h malloc.h free.h malloc_stats.h#Add list of header files

all:	${TESTS} libmalloc.so

clean:
	rm -rf *.o *.so *.h.gch ${TESTS}

libmalloc.so: malloc.o memalign.o realloc.o calloc.o free.o malloc_lib.o malloc_stats.o#add other sources here
	$(CC) $(CFLAGS) -shared -Wl,--unresolved-symbols=ignore-all malloc_lib.o malloc.o memalign.o realloc.o calloc.o free.o  malloc_stats.o -o $@

lib: malloc.o memalign.o realloc.o calloc.o free.o malloc_lib.o malloc_stats.o#add other sources here
	$(CC) $(CFLAGS) -shared -Wl,--unresolved-symbols=ignore-all malloc_lib.o malloc.o memalign.o realloc.o calloc.o free.o  malloc_stats.o -o $@

%: %.c
	$(CC) $(CFLAGS) $< -o $@ -lpthread

# For every XYZ.c file, generate XYZ.o.
%.o: %.c ${HEADERS}
	$(CC) $(CFLAGS) $< -c -o $@

check:	libmalloc.so ${TESTS}
	LD_PRELOAD=`pwd`/libmalloc.so ./t-test1

check1:	libmalloc.so ${TESTS}
	LD_PRELOAD=`pwd`/libmalloc.so ./test1

dist: clean
	dir=`basename $$PWD`; cd ..; tar cvf $$dir.tar ./$$dir; gzip $$dir.tar
