# make file for buzzard library
# $Id: $

CFLAGS = -g -Wall

HEADERS = src/_log.h	\
		src/bzrt_alloc.h	\
		src/bzrt_bytes.h

OBJECTS = bin/bzrt_alloc.o	\
		bin/bzrt_bytes.o

bin/libbzrt.a: $(OBJECTS)
	(cd bin ; ar -rc libbzrt.a *.o )

# TODO: compile rule

bin/bzrt_alloc.o:	src/bzrt_alloc.c $(HEADERS)
	$(CC) $(CFLAGS) src/bzrt_alloc.c -c -o bin/bzrt_alloc.o

bin/bzrt_bytes.o:	src/bzrt_bytes.c $(HEADERS)
	$(CC) $(CFLAGS) src/bzrt_bytes.c -c -o bin/bzrt_bytes.o


# vi: ts=4 sw=4 ai
# *** EOF ***
