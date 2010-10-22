# make file for buzzard lib and tests
# $Id: $

do_it:
	( cd bzrt ; make )
	( cd test ; make )

tags:
	( cd test ; make tags )

# vi: ts=4 sw=4 ai
# *** EOF ***
