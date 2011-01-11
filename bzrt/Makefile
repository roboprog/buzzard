# make file for buzzard library
# $Id: $

# CFLAGS = -g -Wall
CFLAGS = -O3 -Wall

HEADERS = src/_log.h	\
		src/bzrt_alloc.h	\
		src/bzrt_bytes.h	\
		src/bzrt_table.h

OBJECTS = bin/bzrt_alloc.o	\
		bin/bzrt_bytes.o	\
		bin/bzrt_table.o

bin/libbzrt.a: $(OBJECTS)
	(cd bin ; ar -rc libbzrt.a *.o )

# TODO: compile rule
# TODO:  "gang" compile entire library to allow cross-file optimizations

bin/bzrt_alloc.o:	src/bzrt_alloc.c $(HEADERS)
	$(CC) $(CFLAGS) src/bzrt_alloc.c -c -o bin/bzrt_alloc.o

bin/bzrt_bytes.o:	src/bzrt_bytes.c $(HEADERS)
	$(CC) $(CFLAGS) src/bzrt_bytes.c -c -o bin/bzrt_bytes.o

bin/bzrt_table.o:	src/bzrt_table.c $(HEADERS)
	$(CC) $(CFLAGS) src/bzrt_table.c -c -o bin/bzrt_table.o


# vi: ts=4 sw=4 ai
# *** EOF ***
