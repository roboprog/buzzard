/**
 * memory allocation primitives for buzzard.
 *
 * $Id: $
 */

#include <malloc.h>
#include <assert.h>

#include "bzrt_alloc.h"

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

	// TODO: better error handling
	assert( a_stack != NULL);
	assert( *a_stack != NULL);

	frame_start = ( *a_stack)->top;

	( *a_stack)->top += frame_sz;
	if ( ( *a_stack)->top <= ( *a_stack)->size)
		{
		return frame_start;  // === done ===
		}  // reusing existing space?

	// grow stack
	*a_stack = realloc( *a_stack, ( *a_stack)->size);
	assert( *a_stack != NULL);

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
	assert( "TODO: implement this" == NULL);
	}  // _________________________________________________________

// vi: ts=4 sw=4 ai
// *** EOF ***
