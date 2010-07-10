# make file for buzzard library
# $Id: $

CFLAGS = -g -Wall

# TODO: .o file list macro
bin/libbzrt.a: bin/bzrt_alloc.o
	(cd bin ; ar -rc libbzrt.a bzrt_alloc.o )

# TODO: .h file list macro
bin/bzrt_alloc.o:	src/bzrt_alloc.c src/bzrt_alloc.h
	$(CC) $(CFLAGS) src/bzrt_alloc.c -c -o bin/bzrt_alloc.o


# vi: ts=4 sw=4 ai
# *** EOF ***
