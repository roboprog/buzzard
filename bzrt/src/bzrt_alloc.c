/**
 * memory allocation primitives for buzzard.
 *
 * TODO:  idiot needs to decide where to put frame marker,
 *  and how to track its offset.
 *
 *  "data":
 * (HIGH)
 *  	payload 3 <- "top"
 *  	marker 3
 *  	payload 2
 *  	marker 2
 *  	payload 1
 *  	marker 1
 * (LOW)
 *  stack houskeeping fields
 *
 * $Id: $
 */

#include <malloc.h>
#include <assert.h>
#include <stdio.h>

#include "bzrt_alloc.h"

/** stack frame marker */
typedef struct 			t_frame_marker
	{
	size_t				size;			// size of this stack frame,
										//  usable space, excluding overhead
	int					ref_cnt;		// reference count
	size_t				prev_off;		// offset to previous frame
	}					t_frame_marker;

/** return offset of marker structure for top frame */
static  // inline?
size_t					bza_get_top_frame_marker_offset
	(
	t_stack *			stack			// a stack to be displayed,
										//  not null!
	)
	{
	return ( stack->top > sizeof( t_frame_marker) ) ?
			( stack->top - sizeof( t_frame_marker) ) : 0;
	}  // _________________________________________________________

/** return marker structure for top frame */
static  // inline?
t_frame_marker *		bza_get_top_frame_marker
	(
	t_stack *			stack			// a stack to be displayed,
										//  not null!
	)
	{
	size_t				cur_marker_off;

	cur_marker_off = bza_get_top_frame_marker_offset( stack);
	if ( cur_marker_off == 0)
		{
		return NULL;  // === skip ===
		}  // empty?

	return (t_frame_marker *) &( stack->data[ cur_marker_off ]);
	}  // _________________________________________________________

// TODO: macro to enable / disable
/** dump stack / heap structure */
static
void					bza_dump_stack
	(
	t_stack *			stack			// a stack to be displayed
	)
	{
	t_frame_marker *	cur_marker;

	if ( stack == NULL)
		{
		fprintf( stderr, "No stack!");
		return;  // === done ===
		}  // no stack???

	fprintf( stderr, "STK: %d bytes @ %xd\n",
			(int) stack->size, (int) stack);
	// TODO: skip when empty
	cur_marker = bza_get_top_frame_marker( stack);
	if ( cur_marker == NULL)
		{
		return;  // === done ===
		}  // empty?

	fprintf( stderr, "\tFRM: %d b, %d refs (prev %d) @ %xd\n",
			(int) cur_marker->size,
			(int) cur_marker->ref_cnt,
			(int) cur_marker->prev_off,
			(int) cur_marker);
	// TODO: walk down frames...
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
	fputs( "STK: construct:\n", stderr);  // TEMP
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

	// TODO: better error handling
	assert( a_stack != NULL);
	assert( *a_stack != NULL);
	assert( frame_sz >= 0);
	fprintf( stderr, "STK: alloc %d\n", (int) frame_sz);  // TEMP
	bza_dump_stack( *a_stack);  // TEMP

	// TODO:  call (make) stack-walk dumping routine

	// "overname" stuff, let the C optimizer strip out redundancy

	// current, before we added something:
	cur_marker_off = bza_get_top_frame_marker_offset( *a_stack);

	// get the bookkeeping stuff (MIGHT BE RELOCATED!)
	cur_marker = bza_get_top_frame_marker( *a_stack);

	// where will bookkeeping for next frame go:
	next_marker_off = cur_marker->size + ( *a_stack)->top;

	// where will the new payload go:
	next_top = next_marker_off + sizeof( t_frame_marker);

	// how big must stack be to hold new payload + overhead:
	next_size = next_marker_off + frame_sz +
			sizeof( t_frame_marker) + sizeof( t_stack);

	frame_start = ( *a_stack)->top + sizeof( t_frame_marker);

	if ( next_size > ( *a_stack)->size)
		{
		// TODO: minimize allocation calls
		// TODO: determine if alloc is thread safe
		// grow stack
		*a_stack = realloc( *a_stack, ( *a_stack)->size + sizeof( t_stack) );
		assert( *a_stack != NULL);
		}  // new "high water" mark?
	// else:  use/reuse existing space

	( *a_stack)->top = next_top;
	( *a_stack)->size = next_size;

	marker = (t_frame_marker *)
			&( ( *a_stack)->data[ next_marker_off ]);
	marker->size = frame_sz;
	marker->ref_cnt = 1;
	marker->prev_off = cur_marker_off;
	bza_dump_stack( *a_stack);  // TEMP
	return next_top;
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

	// TODO: better error handling
	assert( a_stack != NULL);

	// TODO: check that count actually went to 0

	// TODO: check that frame is actually on top,
	//  rather than sandwiched below an active frame
	assert( stk_frame_off == a_stack->top);

	// at this point, we have a frame with no active references,
	//  and it is on the top of the stack:

	marker = (t_frame_marker *) &( a_stack->data[ a_stack->top ]);
	a_stack->top -= ( marker->size);

	// assert( "TODO: implement this" == NULL);
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
	size_t				cur_marker_off;
	t_frame_marker *	cur_marker;

	// TODO: better error handling
	assert( a_stack != NULL);
	assert( a_stack->top >= stk_frame_off);

	// where is the bookkeeping stuff for the current top:
	cur_marker_off = stk_frame_off - sizeof( t_frame_marker);

	// get the bookkeeping stuff
	cur_marker = (t_frame_marker *)
			&( a_stack->data[ cur_marker_off ]);

	// TODO: better error handling
	assert( cur_marker->ref_cnt > 0);

	return (void *) &( a_stack->data[ stk_frame_off ]);
	}  // _________________________________________________________


// vi: ts=4 sw=4 ai
// *** EOF ***
