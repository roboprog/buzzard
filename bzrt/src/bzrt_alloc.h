/**
 * memory allocation primitives for buzzard.
 *
 * $Id: $
 */

/** stub of a stack instance -- allocation is within a stack */
typedef struct 			t_stack
	{
	size_t				top;			// offset to next available space
	size_t				size;			// total size of stack so far
	char				data[0];		// variable size data buffer
	}					t_stack;

/** create a new (empty) stack */
t_stack *				bza_cons_stack( void)
	;

/** free up a stack (run any needed / practical cleanup) */
void					bza_dest_stack
	(
	t_stack * *			a_stack			// a stack to be torn down
	)
	;

// vi: ts=4 sw=4 ai
// *** EOF ***
