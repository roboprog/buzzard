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
	assert( *a_stack != NULL);

	free( *a_stack);
	*a_stack = NULL;
	}  // _________________________________________________________


// vi: ts=4 sw=4 ai
// *** EOF ***
