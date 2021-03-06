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

/** leaf:  remainder of byte array, with value, are all here */
typedef struct			t_table_leaf
	{
	size_t				key_off;		// (remainder of) key byte array
	size_t				val_off;		// value byte array
	}					t_table_leaf;

/** interior:  interior node at level with more than one possible value */
typedef struct			t_table_interior
	{
	size_t				byte_val_nodes;	// variable size array of t_table
										//  entries, indexed by 0..255
										//  byte value for currrent position
	}					t_table_interior;

/** recursive data structure to interior nodes and leaves. */
typedef struct			t_table
	{
	int					is_leaf;		// true if node in table
										//  (modified trie) is a leaf.
										//  (discriminant for following union)
	union				t_td
		{
		t_table_leaf	leaf;			// leaf type data
		t_table_interior
						interior;		// interior node data (array)
		}				td;				// table data node (union)
	}					t_table;

/** '\0's from which to create "byte array" */
static
size_t					ZERO_BYTES[ 256 ];  // assume 0 filled static data

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
	innards->is_leaf = 1;
	innards->td.leaf.key_off = 0;
	innards->td.leaf.val_off = 0;
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
		bzb_deref( catcher, a_stack, innards->td.leaf.key_off);
		bzb_deref( catcher, a_stack, innards->td.leaf.val_off);
		}  // final reference dropping away?
	// else:  another reference is pending

	bza_deref_stk_frame( catcher, a_stack, table);
	}  // _________________________________________________________

/**
 * Save a key-value pair in an empty (leaf) level in the table.
 * */
static
void					bzt_put_leaf
	(
	jmp_buf *			catcher,		// error handler (or null for immediate death)
	t_stack * *			a_stack,		// a stack on/in which to
										// allocate the frame(s)
										// (which may be relocated!)
	t_table *			innards,		// current node in trie
										//  IMMOVABLE for duration of call
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
	size_t				prev_val;

	innards->is_leaf = 1;
	prev_val = innards->td.leaf.val_off;

	// TODO: dereference prev_val
	// TODO: dereference any old value

	innards->td.leaf.key_off = bzb_from_fixed_mem( catcher, a_stack, key, key_len);
	innards->td.leaf.val_off = bzb_from_fixed_mem( catcher, a_stack, val, val_len);
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
	size_t				cur_key_len;
	size_t				new_leaf;
	t_table *			new_leaf_innards;
	size_t				next_table_level;
	int					cur_byte_idx;
	size_t *			children;

	// TODO:  make this beautiful, not hideous  :-(

	innards = (t_table *) bza_get_frame_ptr( catcher, *a_stack, table);
	if ( innards->is_leaf)
		{
		// TODO: refactor bzt_put_leaf, so less duplication, useful for "split" of key

		if ( innards->td.leaf.key_off == 0)
			{
			bzt_put_leaf( catcher, a_stack, innards, key, key_len, val, val_len);
			return;  // === done ===
			}  // empty leaf node?

		cur_key_len = bzb_size( catcher, *a_stack, innards->td.leaf.key_off);
		if ( ( key_len == cur_key_len) &&
			 ( memcmp( key, 
					bzb_to_asciiz( catcher, *a_stack, innards->td.leaf.key_off),
					cur_key_len) == 0) )
			{
			bzt_put_leaf( catcher, a_stack, innards, key, key_len, val, val_len);
			return;  // === done ===
			}  // update value for existing key in leaf node?

		new_leaf = bza_cons_stk_frame( catcher, a_stack, sizeof( t_table) );
		new_leaf_innards = (t_table *) bza_get_frame_ptr( catcher, *a_stack, table);
		bzt_put_leaf( catcher, a_stack, new_leaf_innards,
			key + 1, key_len - 1,
			val, val_len);

		bzb_deref( catcher, *a_stack, innards->td.leaf.key_off);
		innards->td.leaf.key_off = 0;
		innards->is_leaf = 0;
		cur_byte_idx = (int) ( (unsigned char) key[ 0 ]);
		innards->td.interior.byte_val_nodes = bzb_from_fixed_mem(
				catcher, a_stack,
				(char *) ZERO_BYTES,
				( cur_byte_idx + 1) * sizeof( size_t) );
		children = (size_t *) bzb_to_asciiz( catcher, *a_stack,
			innards->td.interior.byte_val_nodes);
		children[ cur_byte_idx ] = new_leaf;

		assert( "TODO:  create another empty leaf for existing key/val" == NULL);
		// TODO:  create another empty leaf for existing key/val
		}  // current level is a leaf?
	else
		{
		assert( "TODO:  descend from interior node..." == NULL);
		}  // current level is an interior node?


	// TODO: dereference prev_val
	// TODO: dereference any old value

	// TODO: trie/tree

	next_table_level = 0;  // TODO: create / lookup
	bzt_put( catcher, a_stack, next_table_level,
			key + 1, key_len - 1, val, val_len);
	}  // _________________________________________________________

/**
 * Return any value (byte-array containing the value),
 *  matching the given key, from a leaf node in modified trie.
 */
static
size_t					bzt_get_leaf
	(
	jmp_buf *			catcher,		// error handler (or null for immediate death)
	t_stack *			a_stack,		// a stack on/in which to
										// allocate the frame(s)
	t_table *			innards,		// current node in trie
										//  IMMOVABLE for duration of call
	const
	char *				key,			// (remainder of) key data bytes  --
										//  MUST BE "IMMOVABLE"
										//  for the duration of this call
										//  (should not be in given stack)
										//  a copy will be saved at completion
	size_t				key_len			// sizeof key (remainder)
	)
	{
	size_t				cur_key_len;
	size_t				val_off;

	if ( ! innards->td.leaf.key_off)
		{
		return 0;  // === done ===
		}  // nothing stored yet?

	cur_key_len = bzb_size( catcher, a_stack, innards->td.leaf.key_off);
	if ( key_len != cur_key_len)
		{
		return 0;  // === done ===
		}  // different length key?

	val_off = ( memcmp( key, 
				bzb_to_asciiz( catcher, a_stack, innards->td.leaf.key_off),
				cur_key_len) == 0) ?
			innards->td.leaf.val_off : 0;
	return val_off;
	}  // _________________________________________________________

/**
 * Return the offset of the data for the next node in the (modified) trie,
 *  based on the current byte value of the key.
 */
static
size_t					bzt_get_interior
	(
	jmp_buf *			catcher,		// error handler (or null for immediate death)
	t_stack *			a_stack,		// a stack on/in which to
										// allocate the frame(s)
	t_table *			innards,		// current node in trie
										//  IMMOVABLE for duration of call
	char 				key_byte		// current byte from key
										//  to be considered for a match
	)
	{
	size_t				byte_val_nodes;
	int					num_nodes;
	int					node_idx;
	size_t *			nodes;

	byte_val_nodes = innards->td.interior.byte_val_nodes;
	num_nodes = ( bzb_size( catcher, a_stack, byte_val_nodes) /
			sizeof( size_t) );
	node_idx = ( (int) ( (unsigned char) key_byte) );
	if ( node_idx >= num_nodes)
		{
		return 0;  // === fail ===
		}  // no value defined for this byte (position)?

	nodes = (size_t *) bzb_to_asciiz( catcher, a_stack, byte_val_nodes);
	return nodes[ node_idx ];
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
	size_t				next_level;

	innards = (t_table *) bza_get_frame_ptr( catcher, a_stack, table);
	if ( innards->is_leaf)
		{
		return bzt_get_leaf( catcher, a_stack, innards, key, key_len);
		// === done ===
		}  // leaf node?

	// TODO: fixme - key may be consumed on prior levels!
	if ( key_len == 0)
		{
		return 0;  // === fail ===
		}  // no key data?

	next_level = bzt_get_interior( catcher, a_stack, innards, key[ 0 ]);
	if ( ! next_level)
		{
		return 0;  // === fail ===
		}  // no partial match for current byte?

	return 0;  // === fail ===
	}  // _________________________________________________________


// vi: ts=4 sw=4 ai
// *** EOF ***
