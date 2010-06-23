/**
 * memory allocation primitives for buzzard.
 *
 * TODO:  idiot needs to decide where to put frame marker,
 *  and how to track its offset.
 *
 * (LOW)
 *  stack houskeeping fields
 *  "data":
 *  	payload 1
 *  	marker 1
 *  	payload 2
 *  	marker 2
 *  	payload 3
 *  	marker 3 <- "top"
 * (HIGH)
 *
 * $Id: $
 */

#include <malloc.h>
#include <assert.h>

#include "bzrt_alloc.h"

/** stack frame marker */
typedef struct 			t_frame_marker
	{
	size_t				size;			// size of this stack frame
	}					t_frame_marker;

/** create a new (empty) stack */
t_stack *				bza_cons_stack( void)
	{
	t_stack *			stack;

	stack = malloc( sizeof( t_stack) );
	// TODO: better error handling
	assert( stack != NULL);

	stack->size = sizeof( t_stack);
	stack->top = ( stack->data - ( (char *) stack) );
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
	size_t				frame_sz		// size of frame, including overhead
	)
	{
	size_t				frame_start;
	t_frame_marker *	marker;

	// TODO: better error handling
	assert( a_stack != NULL);
	assert( *a_stack != NULL);
	assert( frame_sz >= sizeof( marker) );

	frame_start = ( *a_stack)->top + sizeof( t_frame_marker);

	( *a_stack)->top += frame_sz;
	if ( ( *a_stack)->top > ( *a_stack)->size)
		{
		// grow stack
		*a_stack = realloc( *a_stack, ( *a_stack)->size + sizeof( t_stack) );
		assert( *a_stack != NULL);
		}  // new "high water" mark?
	// else:  use/reuse existing space
	marker = (t_frame_marker *) &( ( *a_stack)->data[
			( *a_stack)->top ]);
	marker->size = frame_sz;
	return frame_start;
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

// vi: ts=4 sw=4 ai
// *** EOF ***
