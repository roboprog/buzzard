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
	;

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
	;

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
	;

/** reference a byte array (increment reference count) */
void					bzb_ref
	(
	jmp_buf *			catcher,		// error handler (or null for immediate death)
	t_stack *			a_stack,		// a stack on/in which 
										// the frame is allocated
	size_t				bytes			// offset of byte array
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
