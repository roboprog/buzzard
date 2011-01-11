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

/** Create an empty table (return offset). */
size_t					bzt_init
	(
	jmp_buf *			catcher,		// error handler (or null for immediate death)
	t_stack * *			a_stack 		// a stack on/in which to
										// allocate the frame(s)
										// (which may be relocated!)
	)
	{
	return 0;  // TODO
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
	// TODO
	}  // _________________________________________________________


// vi: ts=4 sw=4 ai
// *** EOF ***
