/**
 * Byte array primitives for buzzard.
 *
 * $Id: $
 */
/*
    buzzard:  blaze runtime (so far, just a simple memory management library)

    Copyright (C) 2010, Robin R Anderson
    roboprog@yahoo.com
    PO 1608
    Shingle Springs, CA 95682

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "bzrt_bytes.h"

#define DO_LOG	1
#include "_log.h"

/** data structure to manage byte array */
typedef struct			t_bytes
	{
	// TODO: immutable
	size_t				len;			// length in bytes,
										//  includes hidden terminator (\0)
										//  just in case used as asciiz
	char				data[ 0 ];		// variable size buffer for bytes
	}					t_bytes;

/** create a (mutable) byte array from an asciiz string, return offset */
size_t					bzb_from_asciiz
	(
	jmp_buf *			catcher,		// error handler (or null for immediate death)
	t_stack * *			a_stack,		// a stack on/in which to
										// allocate the frame
										// (which may be relocated!)
	const
	char *				src				// C string to be copied
	)
	{
	size_t				str_len;
	size_t				alloc_len;
	size_t				frame;
	t_bytes *			barr;

	assert( src != NULL);
	MLOG_PRINTF( stderr, "*** B-A: from ascii \"%s\"\n", src);
	str_len = strlen( src);

	alloc_len = sizeof( t_bytes) + str_len + 1;
	frame = bza_cons_stk_frame( catcher, a_stack, alloc_len);
	barr = (t_bytes *) bza_get_frame_ptr( catcher, a_stack, frame);
	barr->len = str_len;
	strcpy( barr->data, src);
	return frame;
	}  // _________________________________________________________

/** de-reference a byte array (decrement reference count) */
void					bzb_deref
	(
	jmp_buf *			catcher,		// error handler (or null for immediate death)
	t_stack *			a_stack,		// a stack on/in which 
										// the frame is allocated
	size_t				bytes			// offset of byte array
	)
	{
	// TODO
	}  // _________________________________________________________

/** return the size of the byte array (usable bytes) */
size_t					bzb_size
	(
	jmp_buf *			catcher,		// error handler (or null for immediate death)
	t_stack *			a_stack,		// a stack on/in which 
										// the frame is allocated
	size_t				bytes			// offset of byte array
	)
	{
	// TODO
	}  // _________________________________________________________

/**
 * Return the bytes from a byte array as if it were an asciiz string.
 *  WARNING:  do not use if byte array input has no \0 terminator in it!
 */
const
char *					bzb_to_asciiz
	(
	jmp_buf *			catcher,		// error handler (or null for immediate death)
	t_stack *			a_stack,		// a stack on/in which 
										// the frame is allocated
	size_t				bytes			// offset of byte array
	)
	{
	// TODO
	}  // _________________________________________________________


// vi: ts=4 sw=4 ai
// *** EOF ***
