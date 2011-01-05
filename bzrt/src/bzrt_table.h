/**
 * lookup-table ("map") primitives for buzzard.
 * Note that none of these routines will return or set an error value  --
 * they will either exit or longjmp (throw an exception)
 *
 * $Id: $
 */

#ifndef _BZRT_TABLE_H
#define _BZRT_TABLE_H

#include "bzrt_bytes.h"

/** Create an empty table (return offset). */
size_t					bzt_init
	(
	jmp_buf *			catcher,		// error handler (or null for immediate death)
	t_stack * *			a_stack 		// a stack on/in which to
										// allocate the frame(s)
										// (which may be relocated!)
	)
	;

/** reference a table (increment reference count) */
void					bzt_ref
	(
	jmp_buf *			catcher,		// error handler (or null for immediate death)
	t_stack *			a_stack,		// a stack on/in which 
										// the frame is allocated
	size_t				table			// offset of lookup table
	)
	;

/** de-reference a table (decrement reference count) */
void					bzt_deref
	(
	jmp_buf *			catcher,		// error handler (or null for immediate death)
	t_stack *			a_stack,		// a stack on/in which 
										// the frame is allocated
	size_t				table			// offset of lookup table
	)
	;

/**
 * Save a key-value pair in the table,
 * return any previous value (byte-array containing the value)
 * */
size_t					bzt_put
	(
	jmp_buf *			catcher,		// error handler (or null for immediate death)
	t_stack * *			a_stack,		// a stack on/in which to
										// allocate the frame(s)
										// (which may be relocated!)
	const
	char *				key,			// key data bytes  --
										//  MUST BE "IMMOVABLE"
										//  for the duration of this call
										//  (should not be in given stack)
										//  a copy will be saved at completion
	size_t				key_len,		// sizeof key
	const
	char *				val,			// value data bytes  --
										//  MUST BE "IMMOVABLE"
										//  for the duration of this call
										//  (should not be in given stack)
										//  a copy will be saved at completion
	size_t				val_len			// sizeof val
	)
	;

/**
 * Return any value (byte-array containing the value),
 *  matching the given key.
 * */
size_t					bzt_get
	(
	jmp_buf *			catcher,		// error handler (or null for immediate death)
	t_stack *			a_stack,		// a stack on/in which to
										// allocate the frame(s)
	const
	char *				key,			// key data bytes  --
										//  MUST BE "IMMOVABLE"
										//  for the duration of this call
										//  (should not be in given stack)
										//  a copy will be saved at completion
	size_t				key_len			// sizeof key
	)
	;

size_t

#endif  // BZRT_TABLE_H

// vi: ts=4 sw=4 ai
// *** EOF ***
