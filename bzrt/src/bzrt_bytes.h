/**
 * Byte array primitives for buzzard.
 * Note that none of these routines will return or set an error value  --
 * they will either exit or longjmp (throw an exception)
 *
 * $Id: $
 */

#ifndef _BZRT_BYTES_H
#define _BZRT_BYTES_H

#include "bzrt_alloc.h"

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
	;

/** de-reference a byte array (decrement reference count) */
void					bzb_deref
	(
	jmp_buf *			catcher,		// error handler (or null for immediate death)
	t_stack *			a_stack,		// a stack on/in which 
										// the frame is allocated
	size_t				bytes			// offset of byte array
	)
	;

/** return the size of the byte array (usable bytes) */
size_t					bzb_size
	(
	jmp_buf *			catcher,		// error handler (or null for immediate death)
	t_stack *			a_stack,		// a stack on/in which 
										// the frame is allocated
	size_t				bytes			// offset of byte array
	)
	;

/**
 * Return the bytes from a byte array as if it were an asciiz string.
 *  WARNING:  do not use if byte array input has no \0 terminator in it!
 *  WARNING:  the data may be relocated by a subsequent allocation,
 *  so use and discard this value BEFORE anything else is allocated.
 */
const
char *					bzb_to_asciiz
	(
	jmp_buf *			catcher,		// error handler (or null for immediate death)
	t_stack *			a_stack,		// a stack on/in which 
										// the frame is allocated
	size_t				bytes			// offset of byte array
	)
	;

#endif  // BZRT_BYTES_H

// vi: ts=4 sw=4 ai
// *** EOF ***
