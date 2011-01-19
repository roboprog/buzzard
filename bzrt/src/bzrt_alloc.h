/**
 * memory allocation primitives for buzzard.
 * This library *mostly* acts like a stack,
 * but will release things out of sequence.
 * Note that none of these routines will return or set an error value  --
 * they will either exit or longjmp (throw an exception)
 *
 * $Id: $
 */

#ifndef _BZRT_ALLOC_H
#define _BZRT_ALLOC_H

#include <unistd.h>
#include <setjmp.h>

/** memory allocation handler (internal use only!) */
typedef
void *					( * tf_allocator)
	(
	jmp_buf *			catcher,		// error handler (or null for immediate death)
	void *				existing,		// existing block (if not null)
	size_t				new_size		// number of bytes requested
	)
	;

/** stub of a stack instance -- allocation is within a stack */
typedef struct 			t_stack
	{
	tf_allocator		alloc;			// memory [re]allocator
	size_t				top;			// offset to next available space
	size_t				size;			// total size of stack so far
	char				data[0];		// variable size data buffer
	}					t_stack;

/** create a new (empty) stack */
t_stack *				bza_cons_stack
	(
	jmp_buf *			catcher			// error handler (or null for immediate death)
	)
	;

/** create a new (empty) stack, with "real time" support options */
t_stack *				bza_cons_stack_rt
	(
	jmp_buf *			catcher,		// error handler (or null for immediate death)
	size_t				initial_size,	// initial size of stack
	int					is_fixed		// true if fixed to "initial" size
	)
	;

/** free up a stack (run any needed / practical cleanup) */
void					bza_dest_stack
	(
	jmp_buf *			catcher,		// error handler (or null for immediate death)
	t_stack * *			a_stack			// a stack to be torn down
	)
	;

/** create a new frame on the stack (set reference count to 1) */
size_t					bza_cons_stk_frame
	(
	jmp_buf *			catcher,		// error handler (or null for immediate death)
	t_stack * *			a_stack,		// a stack on/in which to
										// allocate the frame
										// (which may be relocated!)
	size_t				frame_sz		// size of frame, excluding overhead
	)
	;

/** reference a frame on the stack (increment reference count) */
void					bza_ref_stk_frame
	(
	jmp_buf *			catcher,		// error handler (or null for immediate death)
	t_stack *			a_stack,		// a stack on/in which 
										// the frame is allocated
	size_t				stk_frame_off	// offset of stack frame
	)
	;

/** de-reference a frame on the stack (decrement reference count) */
void					bza_deref_stk_frame
	(
	jmp_buf *			catcher,		// error handler (or null for immediate death)
	t_stack *			a_stack,		// a stack on/in which 
										// the frame is allocated
	size_t				stk_frame_off	// offset of stack frame
	)
	;

/** return the reference count of the indicated block */
int						bza_get_ref_count
	(
	jmp_buf *			catcher,		// error handler (or null for immediate death)
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
	jmp_buf *			catcher,		// error handler (or null for immediate death)
	t_stack *			a_stack,		// a stack on/in which 
										// the frame is allocated
	size_t				stk_frame_off	// offset of stack frame
	)
	;

#endif  // BZRT_ALLOC_H

// vi: ts=4 sw=4 ai
// *** EOF ***
