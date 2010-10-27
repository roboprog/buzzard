/**
 * Test harness for runtime library.
 */
/*
    buzzard:  blaze runtime (so far, just a simple memory management library)

    Copyright (C) 2010, Robin R Anderson
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "bzrt_alloc.h"
#include "bzrt_bytes.h"

/**
 * Test (very basic) stack creation / destruction.
 */
static
void					test_stack_init( void)
	{
	t_stack *			stack;

	puts( "\nTest basic stack creation"); fflush( stdout);

	stack = bza_cons_stack( NULL);
	assert( stack != NULL);

	bza_dest_stack( NULL, &stack);
	assert( stack == NULL);
	}  // _________________________________________________________

/**
 * Test stack frame allocation and deallocation.
 */
static
void					test_stack_alloc( void)
	{
	t_stack *			stack;
	size_t 				frames[ 3 ];
	size_t 				tops[ 4 ];

	puts( "\nTest basic stack frame allocation and deallocation"); fflush( stdout);

	stack = bza_cons_stack( NULL);

	tops[ 0 ] = stack->top;

	// throw a few blocks on the stack:

	// puts( "1");  // TEMP
	frames[ 0 ] = bza_cons_stk_frame( NULL, &stack, 64);
	memset( bza_get_frame_ptr( NULL, stack, frames[ 0 ]), 'X', 64);
	tops[ 1 ] = stack->top;
	// assert( ( tops[ 0 ] + 64) <= tops[ 1 ]);
	assert( ( tops[ 0 ] <= frames[ 0 ]) &&
			( frames[ 0 ] <= tops[ 1 ]) );

	// puts( "2");  // TEMP
	frames[ 1 ] = bza_cons_stk_frame( NULL, &stack, 128);
	memset( bza_get_frame_ptr( NULL, stack, frames[ 1 ]), 'Y', 128);
	tops[ 2 ] = stack->top;
	// assert( ( tops[ 1 ] + 128) <= tops[ 2 ]);
	assert( ( tops[ 1 ] <= frames[ 1 ]) &&
			( frames[ 1 ] <= tops[ 2 ]) );

	// puts( "3");  // TEMP
	frames[ 2 ] = bza_cons_stk_frame( NULL, &stack, 32);
	memset( bza_get_frame_ptr( NULL, stack, frames[ 2 ]), 'Z', 32);
	tops[ 3 ] = stack->top;
	// assert( ( tops[ 2 ] + 32) <= tops[ 3 ]);
	assert( ( tops[ 2 ] <= frames[ 2 ]) &&
			( frames[ 2 ] <= tops[ 3 ]) );

	// play with frame release, including out-of-order release
	bza_ref_stk_frame( NULL, stack, frames[ 1 ]);  // extra count on frame 1!

	// puts( "4");  // TEMP
	bza_deref_stk_frame( NULL, stack, frames[ 2 ]);
	assert( stack->top == tops[ 2 ]);

	// puts( "5");  // TEMP
	bza_deref_stk_frame( NULL, stack, frames[ 1 ]);
	assert( stack->top == tops[ 2 ]);  // since frames[ 1 ] is still allocated

	// puts( "6");  // TEMP
	bza_deref_stk_frame( NULL, stack, frames[ 0 ]);
	assert( stack->top == tops[ 2 ]);  // since frames[ 1 ] is still allocated
	bza_deref_stk_frame( NULL, stack, frames[ 1 ]);  // get rid of "extra" count
	assert( stack->top == tops[ 0 ]);  // double pop

	bza_dest_stack( NULL, &stack);
	}  // _________________________________________________________

/**
 * Test "real time" stack frame allocation and deallocation.
 *  "Real time" means that sub-allocations will run in deterministic time,
 *  but of course we have no control over external OS scheduling.
 */
static
void					test_rt_stack_alloc( void)
	{
	const
	size_t				K256 = ( 1 << 18);
	const
	char *				datums = "XYZ";

	t_stack *			stack;
	size_t 				frames[ 3 ];
	char *				data[ 3 ];
	int					idx;
	jmp_buf				catcher;
	int					is_err;

	puts( "\nTest real-time support frame allocation and deallocation"); fflush( stdout);

	stack = bza_cons_stack_rt( NULL, ( 1 << 20), 1);  // 1 MB

	// allocation and deallocation

	for ( idx = 0; idx < 3; idx++)

		{
		frames[ idx ] = bza_cons_stk_frame( NULL, &stack, K256);
		data[ idx ] = bza_get_frame_ptr( NULL, stack, frames[ idx ]);
		memset( data[ idx ], datums[ idx ], K256);
		}  // allocate and init each chunk

	for ( idx = 0; idx < 3; idx++)

		{
		assert( data[ idx ][ 0 ] == datums[ idx ]);
		assert( data[ idx ][ K256 - 1 ] == datums[ idx ]);
		}  // test content of each block

	// overallocate, test error handling

	is_err = setjmp( catcher);
	if ( ! is_err)
		{
		bza_cons_stk_frame( &catcher, &stack, K256 * 2);
		assert( "Error check failed, this should not be reached" == NULL);
		}  // initial "try" to overallocate?
	// else:  falling through from the error check + longjmp
	for ( idx = 0; idx < 3; idx++)

		{
		assert( data[ idx ][ 0 ] == datums[ idx ]);
		assert( data[ idx ][ K256 - 1 ] == datums[ idx ]);
		}  // *re*-test content of each block


	// clean up

	bza_deref_stk_frame( NULL, stack, frames[ 2 ]);
	bza_deref_stk_frame( NULL, stack, frames[ 1 ]);
	bza_deref_stk_frame( NULL, stack, frames[ 0 ]);

	bza_dest_stack( NULL, &stack);

	// TODO: take timings, figure how to test acceptability
	}  // _________________________________________________________

/**
 * Test basic (immutable) byte array functionality
 */
static
void					test_byte_array( void)
	{
	static const
	char *				TEST_STR = "Testing, 123";

	t_stack *			stack;
	size_t				barr;

	puts( "\nTest immutable byte array use"); fflush( stdout);

	stack = bza_cons_stack( NULL);

	// asciiz array init / access

	barr = bzb_from_asciiz( NULL, &stack, TEST_STR);
	assert( bzb_size( NULL, stack, barr) == strlen( TEST_STR) );
	assert( strcmp( bzb_to_asciiz( NULL, stack, barr), TEST_STR) == 0);
	bzb_deref( NULL, stack, barr);

	// TODO: test sub-array access

	// TODO: test array concatenation

	bza_dest_stack( NULL, &stack);
	}  // _________________________________________________________

/**
 * Drive tests.
 * TODO: xunit or something like that (but exit-on-failure for now)
 */
int						main
	(
	int					argc,
	char *				argv []
	)
	{
	test_stack_init();
	test_stack_alloc();
	test_rt_stack_alloc();

	test_byte_array();
	// TODO: test_mutable_byte_array();

	// TODO: basic I/O

	// TODO: reference frame management

	// TODO: task messaging

	// TODO: thread as task

	return 0;
	}  // _________________________________________________________

// vi: ts=4 sw=4 ai
// *** EOF ***
