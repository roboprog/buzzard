/**
 * Lookup table primitives for buzzard.
 *
 * $Id: $
 */
/*
    buzzard:  blaze runtime (so far, just a simple memory management library)

    Copyright (C) 2010, 2011, Robin R Anderson
    roboprog@yahoo.com
    PO 1608
    Shingle Springs, CA 95682

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "bzrt_table.h"

// #define DO_LOG	1
#include "_log.h"

typedef struct			t_table
	{
	size_t				key_off;		// TODO: real tracking info
	size_t				val_off;		// TODO: real tracking info
	}					t_table;

/** Create an empty table (return offset). */
size_t					bzt_init
	(
	jmp_buf *			catcher,		// error handler (or null for immediate death)
	t_stack * *			a_stack 		// a stack on/in which to
										// allocate the frame(s)
										// (which may be relocated!)
	)
	{
	size_t				table;
	t_table *			innards;

	table = bza_cons_stk_frame( catcher, a_stack, sizeof( t_table) );
	innards = (t_table *) bza_get_frame_ptr( catcher, *a_stack, table);
	innards->key_off = 0;  // TODO:  real state tracking
	innards->val_off = 0;  // TODO:  real state tracking
	return table;
	}  // _________________________________________________________

/** de-reference a table (decrement reference count) */
void					bzt_deref
	(
	jmp_buf *			catcher,		// error handler (or null for immediate death)
	t_stack *			a_stack,		// a stack on/in which 
										// the frame is allocated
	size_t				table			// offset of lookup table
	)
	{
	t_table *			innards;

	// check for 1 -> 0 transition, release contents
	if ( bza_get_ref_count( catcher, a_stack, table) == 1)
		{
		// TODO: recursive deallocation of multiple values
		// TODO:  check for nothing in fact stored (likely handled by real implementation anyway, unlike placholder)
		innards = (t_table *) bza_get_frame_ptr( catcher, a_stack, table);
		bzb_deref( catcher, a_stack, innards->key_off);
		bzb_deref( catcher, a_stack, innards->val_off);
		}  // final reference dropping away?
	// else:  another reference is pending

	bza_deref_stk_frame( catcher, a_stack, table);
	}  // _________________________________________________________

/**
 * Save a key-value pair in the table.
 * */
void					bzt_put
	(
	jmp_buf *			catcher,		// error handler (or null for immediate death)
	t_stack * *			a_stack,		// a stack on/in which to
										// allocate the frame(s)
										// (which may be relocated!)
	size_t				table,			// offset of lookup table
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
	{
	t_table *			innards;
	size_t				prev_val;

	innards = (t_table *) bza_get_frame_ptr( catcher, *a_stack, table);
	prev_val = innards->val_off;  // TODO: real source for value
	// TODO: dereference prev_val

	// TODO: dereference any old value

	innards->val_off = bzb_from_fixed_mem( catcher, a_stack, val, val_len);
	}  // _________________________________________________________

/**
 * Return any value (byte-array containing the value),
 *  matching the given key.
 * */
size_t					bzt_get
	(
	jmp_buf *			catcher,		// error handler (or null for immediate death)
	t_stack *			a_stack,		// a stack on/in which to
										// allocate the frame(s)
	size_t				table,			// offset of lookup table
	const
	char *				key,			// key data bytes  --
										//  MUST BE "IMMOVABLE"
										//  for the duration of this call
										//  (should not be in given stack)
										//  a copy will be saved at completion
	size_t				key_len			// sizeof key
	)
	{
	t_table *			innards;

	innards = (t_table *) bza_get_frame_ptr( catcher, a_stack, table);
	return innards->val_off;
	}  // _________________________________________________________


// vi: ts=4 sw=4 ai
// *** EOF ***
