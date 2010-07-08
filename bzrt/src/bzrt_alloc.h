/**
 * memory allocation primitives for buzzard.
 * This library *mostly* acts like a stack,
 * but will release things out of sequence.
 * Note that none of these routines will return or set an error value  --
 * they will either exit or longjmp (throw an exception)
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

/** create a new frame on the stack (set reference count to 1) */
size_t					bza_cons_stk_frame
	(
	t_stack * *			a_stack,		// a stack on/in which to
										// allocate the frame
										// (which may be relocated!)
	size_t				frame_sz		// size of frame, excluding overhead
	)
	;

/** reference a frame on the stack (increment reference count) */
void					bza_ref_stk_frame
	(
	t_stack *			a_stack,		// a stack on/in which 
										// the frame is allocated
	size_t				stk_frame_off	// offset of stack frame
	)
	;

/** de-reference a frame on the stack (decrement reference count) */
void					bza_deref_stk_frame
	(
	t_stack *			a_stack,		// a stack on/in which 
										// the frame is allocated
	size_t				stk_frame_off	// offset of stack frame
	)
	;

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
	;


// vi: ts=4 sw=4 ai
// *** EOF ***
