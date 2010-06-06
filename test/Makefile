# make file for buzzard tests
# $Id: $

CFLAGS = -I../bzrt/src

bin/test: src/main.c
	$(CC) $(CFLAGS) src/main.c -L../bzrt/bin -lbzrt -o bin/test
	bin/test

# vi: ts=4 sw=4 ai
# *** EOF ***
