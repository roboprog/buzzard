# make file for buzzard tests
# $Id: $

CFLAGS = -g -I../bzrt/src -Wall

run_test: bin/test
	bin/test

bin/test: src/main.c ../bzrt/bin/libbzrt.a
	$(CC) $(CFLAGS) src/main.c -L../bzrt/bin -lbzrt -o bin/test

# vi: ts=4 sw=4 ai
# *** EOF ***
