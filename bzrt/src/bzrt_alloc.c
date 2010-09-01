/**
 * memory allocation primitives for buzzard.
 *
 * Memory layout within a "stack" (not strictly used as a stack):
 *  "data":
 * (HIGH)
 *  	<- "top"
 *  	marker 3
 *  	payload 3
 *  	marker 2
 *  	payload 2
 *  	marker 1
 *  	payload 1
 * (LOW)
 *  stack houskeeping fields
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

#include <malloc.h>
#include <assert.h>
#include <stdio.h>

#include "bzrt_alloc.h"

#define DO_LOG	1

#ifdef DO_LOG
	#define MLOG_PUTS( s) fputs( (s), stderr)

	// TODO: something a bit more clever (var-arg macro?)
	#define MLOG_PRINTF	fprintf
#else
	#define MLOG_PUTS( s) /* */

	#define MLOG_PRINTF	//
#endif  // DO_LOG defined?

/** stack frame marker */
typedef struct 			t_frame_marker
	{
	size_t				size;			// size of this stack frame,
										//  usable space, excluding overhead
	int					ref_cnt;		// reference count
	// TODO: remove redundant field (size vs prev offset)
	size_t				prev_off;		// offset to previous frame
	}					t_frame_marker;

/** return offset of marker structure for top frame */
static  // inline?
size_t					bza_get_top_frame_marker_offset
	(
	t_stack *			stack			// a stack to be accessed,
										//  not null!
	)
	{
	// the frame marker is just underneathh the stack top, unless empty
	return ( stack->top > sizeof( t_frame_marker) ) ?
			( stack->top - sizeof( t_frame_marker) ) : 0;
	}  // _________________________________________________________

/**
 * Return marker structure for indicated frame.
 *  WARNING:  this is a volatile value,
 *  which will often be invalidated by a stack resize.
 */
static  // inline?
t_frame_marker *		bza_get_frame_marker
	(
	t_stack *			stack,			// a stack to be displayed,
										//  not null!
	size_t				marker_off		// offset to desired frame marker
	)
	{
	if ( marker_off == 0)
		{
		return NULL;  // === skip ===
		}  // empty?

	return (t_frame_marker *) &( stack->data[ marker_off ]);
	}  // _________________________________________________________

// TODO: macro to enable / disable
/** dump stack / heap structure */
static
void					bza_dump_stack
	(
	t_stack *			stack			// a stack to be displayed
	)
	{
	size_t				marker_off;
	t_frame_marker *	cur_marker;

	if ( stack == NULL)
		{
		MLOG_PRINTF( stderr, "No stack!");
		return;  // === done ===
		}  // no stack???

	MLOG_PRINTF( stderr, "STK: %d bytes @ %x\n",
			(int) stack->size, (int) stack);
	for ( marker_off = bza_get_top_frame_marker_offset( stack);
		  marker_off != 0;
		  marker_off = cur_marker->prev_off)

		{
		cur_marker = bza_get_frame_marker( stack, marker_off);
		MLOG_PRINTF( stderr, "\tFRM: %d b, %d refs (prev %d) @ %d\n",
				(int) cur_marker->size,
				(int) cur_marker->ref_cnt,
				(int) cur_marker->prev_off,
				(int) marker_off);
		}  // dump each frame

	}  // _________________________________________________________

/** create a new (empty) stack */
t_stack *				bza_cons_stack( void)
	{
	t_stack *			stack;

	stack = malloc( sizeof( t_stack) );
	// TODO: better error handling
	assert( stack != NULL);

	// TODO: define boundary better, so I can recognize an empty stack

	stack->size = sizeof( t_stack);
	stack->top = 0;
	MLOG_PUTS( "*** STK: construct:\n");
	bza_dump_stack( stack);  // TEMP
	return stack;
	}  // _________________________________________________________

/** free up a stack (run any needed / practical cleanup) */
void					bza_dest_stack
	(
	t_stack * *			a_stack			// a stack to be torn down
	)
	{
	// TODO: better error handling
	assert( a_stack != NULL);
	assert( *a_stack != NULL);

	free( *a_stack);
	*a_stack = NULL;
	}  // _________________________________________________________

/** create a new frame on the stack (set reference count to 1) */
size_t					bza_cons_stk_frame
	(
	t_stack * *			a_stack,		// a stack on/in which to
										// allocate the frame
										// (which may be relocated!)
	size_t				frame_sz		// size of frame, excluding overhead
	)
	{
	size_t				cur_marker_off;
	t_frame_marker *	cur_marker;
	size_t				next_marker_off;
	size_t				next_top;
	size_t				next_size;
	size_t				frame_start;
	t_frame_marker *	marker;
	void *				ptr;
	size_t				sz;

	// TODO: better error handling
	assert( a_stack != NULL);
	assert( *a_stack != NULL);
	assert( frame_sz >= 0);
	MLOG_PRINTF( stderr, "*** STK: alloc %d\n", (int) frame_sz);  // TEMP
	bza_dump_stack( *a_stack);  // TEMP

	// TODO:  call (make) stack-walk dumping routine

	// "overname" stuff, let the C optimizer strip out redundancy

	// current, before we added something:
	cur_marker_off = bza_get_top_frame_marker_offset( *a_stack);

	// get the bookkeeping stuff (MIGHT BE RELOCATED!)
	cur_marker = bza_get_frame_marker( *a_stack, cur_marker_off);

	// where will bookkeeping for next frame go:
	next_marker_off = ( cur_marker != NULL) ?
			( frame_sz + ( *a_stack)->top) :
			frame_sz;

	// where will the next payload (on subsequent call) go:
	next_top = next_marker_off + sizeof( t_frame_marker);

	// how big must stack be to hold new payload + overhead:
	next_size = next_top;

	frame_start = ( *a_stack)->top + sizeof( t_frame_marker);

	if ( next_size > ( *a_stack)->size)
		{
		// TODO: minimize allocation calls
		// TODO: determine if alloc is thread safe
		// grow stack
		// (more excess debug visibility vars)
		ptr = *a_stack;
		sz = next_size + sizeof( t_stack);
		ptr = realloc( ptr, sz);
		*a_stack = ptr;
		assert( *a_stack != NULL);  // TODO: better error check
		}  // new "high water" mark?
	// else:  use/reuse existing space

	( *a_stack)->top = next_top;
	( *a_stack)->size = next_size;

	marker = bza_get_frame_marker( *a_stack, next_marker_off);
	marker->size = frame_sz;
	marker->ref_cnt = 1;
	marker->prev_off = cur_marker_off;  // 0 for first thing added
	bza_dump_stack( *a_stack);  // TEMP
	return next_marker_off;
	}  // _________________________________________________________

/** reference a frame on the stack (increment reference count) */
void					bza_ref_stk_frame
	(
	t_stack *			a_stack,		// a stack on/in which 
										// the frame is allocated
	size_t				stk_frame_off	// offset of stack frame
	)
	{
	t_frame_marker *	marker;
	size_t				marker_off;
	t_frame_marker *	cur_marker;

	// TODO: better error handling
	assert( a_stack != NULL);
	MLOG_PRINTF( stderr, "*** STK: ref frame off %d\n", (int) stk_frame_off);  // TEMP
	bza_dump_stack( a_stack);  // TEMP

	// increment count
	marker = bza_get_frame_marker( a_stack, stk_frame_off);
	assert( marker->ref_cnt > 0);
	( marker->ref_cnt)++;

	// TODO: check for counter overflow?

	bza_dump_stack( a_stack);  // TEMP
	}  // _________________________________________________________

/** de-reference a frame on the stack (decrement reference count) */
void					bza_deref_stk_frame
	(
	t_stack *			a_stack,		// a stack on/in which 
										// the frame is allocated
	size_t				stk_frame_off	// offset of stack frame
	)
	{
	t_frame_marker *	marker;
	size_t				marker_off;
	t_frame_marker *	cur_marker;

	// TODO: better error handling
	assert( a_stack != NULL);
	MLOG_PRINTF( stderr, "*** STK: deref frame off %d\n", (int) stk_frame_off);  // TEMP
	bza_dump_stack( a_stack);  // TEMP

	// decrement count
	marker = bza_get_frame_marker( a_stack, stk_frame_off);
	assert( marker->ref_cnt > 0);
	( marker->ref_cnt)--;
	if ( marker->ref_cnt > 0)
		{
		return;  // === done ===
		}  // frame still in use?

	if ( stk_frame_off != bza_get_top_frame_marker_offset( a_stack) )
		{
		return;  // === done ===
		}  // frame not top-most?

	// so, the top frame has no remaining references, pop all we can:

	for ( marker_off = bza_get_top_frame_marker_offset( a_stack);
		  marker_off != 0;
		  marker_off = cur_marker->prev_off)

		{
		cur_marker = bza_get_frame_marker( a_stack, marker_off);
		if ( cur_marker->ref_cnt > 0)
			{
			break;  // === done ===
			}  // still in use?

		// discard *this* frame
		a_stack->top = ( cur_marker->prev_off > 0) ?
			( cur_marker->prev_off + sizeof( t_frame_marker) ) : 0;
		}  // walk down each frame

	bza_dump_stack( a_stack);  // TEMP
	}  // _________________________________________________________

/**
 * return a pointer to the payload data in the indicated frame.
 *  WARNING:  the data may be relocated by a subsequent allocation,
 *  so use and discard this value BEFORE anything else is allocated.
 */
void *					bza_get_frame_ptr
	(
	t_stack *			a_stack,		// a stack on/in which 
										// the frame is allocated
	size_t				stk_frame_off	// offset of stack frame
	)
	{
	t_frame_marker *	cur_marker;
	int					data_off;

	MLOG_PRINTF( stderr, "*** STK: ptr for frame off %d\n", (int) stk_frame_off);  // TEMP

	// TODO: better error handling
	assert( a_stack != NULL);
	assert( ( 0 < stk_frame_off) && ( stk_frame_off < a_stack->top) );

	// get the bookkeeping stuff
	cur_marker = bza_get_frame_marker( a_stack, stk_frame_off);
	MLOG_PRINTF( stderr, "\tFRM: %d b, %d refs (prev %d) @ %d\n",  // TEMP
			(int) cur_marker->size,
			(int) cur_marker->ref_cnt,
			(int) cur_marker->prev_off,
			(int) stk_frame_off);

	// TODO: better error handling
	assert( cur_marker->ref_cnt > 0);

	// use *signed* arithmetic for offset:
	data_off = ( (int) stk_frame_off) - ( (int) cur_marker->size);
	MLOG_PRINTF( stderr, "\tDATA @ %d\n", data_off);  // TEMP
	fflush( stderr);  // TODO: make flushing debug log routines
	return (void *) &( a_stack->data[ data_off ]);
	}  // _________________________________________________________


// vi: ts=4 sw=4 ai
// *** EOF ***
