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
	size_t				len;			// length in usable bytes,
										//  excludes hidden terminator (\0)
										//  added just in case used as asciiz
	size_t				alloc;			// size allocated,
										//  may be larger than len.
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
	size_t				bytes;
	t_bytes *			barr;

	assert( src != NULL);
	MLOG_PRINTF( stderr, "*** B-A: from ascii \"%s\"\n", src);
	str_len = strlen( src);

	alloc_len = sizeof( t_bytes) + str_len + 1;
	bytes = bza_cons_stk_frame( catcher, a_stack, alloc_len);
	barr = (t_bytes *) bza_get_frame_ptr( catcher, *a_stack, bytes);
	barr->len = str_len;
	barr->alloc = str_len + 1;
	strcpy( barr->data, src);
	return bytes;
	}  // _________________________________________________________

/** create a (mutable) byte array buffer with an initial size */
size_t					bzb_init_size
	(
	jmp_buf *			catcher,		// error handler (or null for immediate death)
	t_stack * *			a_stack,		// a stack on/in which to
										// allocate the frame
										// (which may be relocated!)
	size_t				size			// initial size of buffer
	)
	{
	size_t				alloc_len;
	size_t				bytes;
	t_bytes *			barr;

	MLOG_PRINTF( stderr, "*** B-A: reserved size %d\n", (int) size);

	assert( size > 0);

	alloc_len = sizeof( t_bytes) + size;
	bytes = bza_cons_stk_frame( catcher, a_stack, alloc_len);
	barr = (t_bytes *) bza_get_frame_ptr( catcher, *a_stack, bytes);
	barr->len = 0;
	barr->alloc = size;
	barr->data[ 0 ] = '\0';
	return bytes;
	}  // _________________________________________________________

static
void					calc_bounds
	(
	jmp_buf *			catcher,		// error handler (or null for immediate death)
	size_t				src_size,		// total size of source
	int					from,			// starting point (if >= 0)
	int					len,			// size to copy (if >= 0)
	int *				start,			// 0 based index of first byte,
										//  not null
	int *				stop,			// 0 based index of final byte,
										//  not null
	int *				eff_len			// total byte count to copy,
										//  not null
	)
	{
	int					mode;

	mode =	( ( from >= 0) ? 2 : 0) +
			( ( len >= 0) ? 1 : 0);
	switch ( mode)
		{
		case 3 :
				*start = from;
				*eff_len = len;
			break;
		case 2 :
				*start = from;
				*eff_len = src_size - *start;
			break;
		case 1 :
				*start = src_size - len;
				*eff_len = len;
			break;
		default :
				assert( "start offset or length must be supplied" == NULL);
			break;
		}  // which arg mode?
	*stop = *start + ( *eff_len - 1);  // TODO:  check for off-by-one

	// bounds check!

	if ( ! ( ( 0 <= *start) && ( *stop < src_size) ) )
		{
		if ( catcher != NULL)
			{
			longjmp( *catcher, 1);  // === abort ===
			}  // error handler?

		assert( "start/stop out of bounds" == NULL);
		}  // out of bounds (start or stop)?

	}  // _________________________________________________________

/** create a (mutable) byte arrray from another byte array subrange */
size_t					bzb_subarray
	(
	jmp_buf *			catcher,		// error handler (or null for immediate death)
	t_stack * *			a_stack,		// a stack on/in which to
										// allocate the frame
										// (which may be relocated!)
	size_t				src,			// byte array from which to
										//  copy the subrange
	int					from,			// starting point (if >= 0)
	int					len				// size to copy (if >= 0)
	)
	{
	// TODO:  eliminate redundant vars
	int					start;
	int					stop;
	int					eff_len;
	size_t				alloc_len;
	size_t				bytes;
	t_bytes *			barr;
	char *				dptr;
	const
	char *				sptr;
	int					sidx;

	MLOG_PRINTF( stderr, "*** B-A: from subarray @%d[ %d, %d ]\n", (int) src, from, len);

	calc_bounds( catcher, bzb_size( catcher, *a_stack, src), from, len,
			&start, &stop, &eff_len);
	alloc_len = sizeof( t_bytes) + ( stop - start) + 2;
	bytes = bza_cons_stk_frame( catcher, a_stack, alloc_len);
	barr = (t_bytes *) bza_get_frame_ptr( catcher, *a_stack, bytes);
	barr->len = eff_len;
	barr->alloc = eff_len + 1;
	dptr = &( barr->data[ 0 ]);
	sptr = bzb_to_asciiz( catcher, *a_stack, src);  // just get the pointer
	// TODO: tune this tight
	// or, just replace it with memcpy call!
	for ( sidx = start; sidx <= stop; sidx++)

		{
		*dptr = sptr[ sidx ];
		dptr++;
		}  // copy each byte

	*dptr = '\0';

	return bytes;
	}  // _________________________________________________________

/** create a (mutable) byte arrray by concatenating other bytes arrays */
size_t					bzb_concat
	(
	jmp_buf *			catcher,		// error handler (or null for immediate death)
	t_stack * *			a_stack,		// a stack on/in which to
										// allocate the frame
										// (which may be relocated!)
	size_t *			srcs			// array of byte arrray (offsets),
										//  terminated by a 0 entry.
	)
	{
	size_t				src_len;
	const
	size_t *			src_ptr;
	size_t				alloc_len;
	size_t				bytes;
	t_bytes *			barr;
	char *				dptr;

	assert( srcs != NULL);
	MLOG_PRINTF( stderr, "*** B-A: concat arrays @%d...\n", (int) srcs[ 0 ]);

	src_len = 0;
	for ( src_ptr = srcs; *src_ptr; src_ptr++)

		{
		assert( ( *src_ptr) <= ( ( *a_stack)->top) );
		src_len += bzb_size( catcher, *a_stack, *src_ptr);
		}  // sum each src size

	alloc_len = sizeof( t_bytes) + src_len + 1;
	bytes = bza_cons_stk_frame( catcher, a_stack, alloc_len);
	barr = (t_bytes *) bza_get_frame_ptr( catcher, *a_stack, bytes);
	barr->len = src_len;
	barr->alloc = src_len + 1;
	dptr = &( barr->data[ 0 ]);
	for ( src_ptr = srcs; *src_ptr; src_ptr++)

		{
		src_len = bzb_size( catcher, *a_stack, *src_ptr);
		memcpy( dptr, bzb_to_asciiz( catcher, *a_stack, *src_ptr), src_len);
		dptr += src_len;
		}  // sum each src size

	*dptr = '\0';

	return bytes;
	}  // _________________________________________________________

/**
 * Modify or recreate, as needed, the given byte array
 *  by inserting / overwriting the specified byte range
 *  with bytes from a second array.
 *  IMPORTANT:  deref the dst arg after this call,
 *  then use return value in its place  --
 *  this may or may not be the same storage area,
 *  but the reference cound will be adjusted as needed.
 */
size_t					bzb_splice
	(
	jmp_buf *			catcher,		// error handler (or null for immediate death)
	t_stack * *			a_stack,		// a stack on/in which to
										// allocate the frame
										// (which may be relocated!)
	size_t				dst,			// byte array into which to
										//  put the new data.
	int					dfrom,			// starting point (if >= 0)
	int					dlen,			// size to copy (if >= 0)
	size_t				src,			// byte array from which to
										//  copy the subrange
	int					sfrom,			// starting point (if >= 0)
	int					slen			// size to copy (if >= 0)
	)
	{
	size_t				d_size;
	int					d_start;
	int					d_stop;
	int					d_eff_len;
	int					s_start;
	int					s_stop;
	int					s_eff_len;
	size_t				tot_len;
	size_t				alloc_len;
	size_t				bytes;
	t_bytes *			barr;
	char *				dptr;

	MLOG_PRINTF( stderr, "*** B-A: splice @%d[ %d, %d ] <- @%d[ %d, %d ]\n", (int) dst, dfrom, dlen, (int) src, sfrom, slen);

	// TODO: deal with src == dst case
	assert( dst != src);

	// determine the extent of the replacement within the destination buf:
	barr = (t_bytes *) bza_get_frame_ptr( catcher, *a_stack, dst);
	d_size = barr->alloc;
	calc_bounds( catcher, d_size, dfrom, dlen,
			&d_start, &d_stop, &d_eff_len);

	// determine the extent to be grabbed from the source buf:
	calc_bounds( catcher, bzb_size( catcher, *a_stack, src), sfrom, slen,
			&s_start, &s_stop, &s_eff_len);

	tot_len = ( d_size - d_eff_len) + s_eff_len;
	if ( tot_len <= d_size)
		{
		bytes = dst;
		// "barr" assigned above
		bzb_ref( catcher, *a_stack, bytes);
		}  // room still?
	else
		{
		// TODO:  grow a reasonable amount, not just exactly enough
		alloc_len = sizeof( t_bytes) + tot_len;
		bytes = bza_cons_stk_frame( catcher, a_stack, alloc_len);
		barr = (t_bytes *) bza_get_frame_ptr( catcher, *a_stack, bytes);
		barr->alloc = tot_len + 1;
		}  // need to grow a bigger buffer?
	barr->len = tot_len;
	dptr = &( barr->data[ 0 ]);
	memcpy( dptr,
			bzb_to_asciiz( catcher, *a_stack, dst),
			d_start);
	dptr += d_start;
	memcpy( dptr,
			&( bzb_to_asciiz( catcher, *a_stack, src)[ s_start ]),
			s_eff_len);
	dptr += s_eff_len;
	memcpy( dptr,
			&( bzb_to_asciiz( catcher, *a_stack, dst)[ d_stop + 1 ]),
			( d_size - d_stop) );
	dptr += ( d_size - d_stop);
	*dptr = '\0';

	return bytes;
	}  // _________________________________________________________

/** reference a byte array (increment reference count) */
void					bzb_ref
	(
	jmp_buf *			catcher,		// error handler (or null for immediate death)
	t_stack *			a_stack,		// a stack on/in which 
										// the frame is allocated
	size_t				bytes			// offset of byte array
	)
	{
	MLOG_PRINTF( stderr, "*** B-A: ref frame off %d\n", (int) bytes);  // TEMP
	bza_ref_stk_frame( catcher, a_stack, bytes);
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
	MLOG_PRINTF( stderr, "*** B-A: deref frame off %d\n", (int) bytes);  // TEMP
	bza_deref_stk_frame( catcher, a_stack, bytes);
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
	t_bytes *			barr;

	barr = (t_bytes *) bza_get_frame_ptr( catcher, a_stack, bytes);
	return barr->len;
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
	t_bytes *			barr;

	barr = (t_bytes *) bza_get_frame_ptr( catcher, a_stack, bytes);
	// note that we always have an extra '\0' just pass the end of the array
	return barr->data;
	}  // _________________________________________________________


// vi: ts=4 sw=4 ai
// *** EOF ***
