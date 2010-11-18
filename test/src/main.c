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
	const
	size_t				TS_LEN = strlen( TEST_STR);

	t_stack *			stack;
	size_t				empty_top;
	size_t				barr;
	size_t				head;
	size_t				tail;
	size_t				srcs[ 4 ];
	size_t				big;
	jmp_buf				catcher;
	int					is_err;

	puts( "\nTest immutable byte array use"); fflush( stdout);

	stack = bza_cons_stack( NULL);
	empty_top = stack->top;

	// asciiz array init / access

	barr = bzb_from_asciiz( NULL, &stack, TEST_STR);
	assert( bzb_size( NULL, stack, barr) == TS_LEN);
	assert( strcmp( bzb_to_asciiz( NULL, stack, barr), TEST_STR) == 0);
	bzb_deref( NULL, stack, barr);
	assert( stack->top == empty_top);

	// test sub-array access

	barr = bzb_from_asciiz( NULL, &stack, TEST_STR);

	head = bzb_subarray( NULL, &stack, barr, 0, 3);
	assert( memcmp( bzb_to_asciiz( NULL, stack, head), TEST_STR, 3) == 0);
	bzb_deref( NULL, stack, head);

	tail = bzb_subarray( NULL, &stack, barr, 9, -1);
	assert( memcmp( bzb_to_asciiz( NULL, stack, tail), TEST_STR + 9, 3) == 0);
	bzb_deref( NULL, stack, tail);

	tail = bzb_subarray( NULL, &stack, barr, -1, 3);
	assert( memcmp( bzb_to_asciiz( NULL, stack, tail), TEST_STR + 9, 3) == 0);
	bzb_deref( NULL, stack, tail);

	bzb_deref( NULL, stack, barr);
	assert( stack->top == empty_top);

	// test array concatenation

	barr = bzb_from_asciiz( NULL, &stack, TEST_STR);
	srcs[ 0 ] = srcs[ 1 ] = srcs[ 2 ] = barr;
	srcs[ 3 ] = 0;
	big = bzb_concat( NULL, &stack, srcs);
	bzb_deref( NULL, stack, barr);
	assert( bzb_size( NULL, stack, big) == ( 3 * TS_LEN) );
	assert( memcmp(
			&( bzb_to_asciiz( NULL, stack, big)[ 0 ]),
			TEST_STR, TS_LEN) == 0);
	assert( memcmp(
			&( bzb_to_asciiz( NULL, stack, big)[ TS_LEN ]),
			TEST_STR, TS_LEN) == 0);
	assert( memcmp(
			&( bzb_to_asciiz( NULL, stack, big)[ 2 * TS_LEN ]),
			TEST_STR, TS_LEN) == 0);
	bzb_deref( NULL, stack, big);
	assert( stack->top == empty_top);

	// TODO: test update attempt to immutable array

	// test bounds checking

	barr = bzb_from_asciiz( NULL, &stack, TEST_STR);
	is_err = setjmp( catcher);
	if ( ! is_err)
		{
		bzb_subarray( &catcher, &stack, barr, 0, 1000);
		assert( "Error check failed, this should not be reached" == NULL);
		}  // initial "try" to overallocate?
	// else:  falling through from the error check + longjmp
	is_err = setjmp( catcher);
	if ( ! is_err)
		{
		bzb_subarray( &catcher, &stack, barr, 1000, 1);
		assert( "Error check failed, this should not be reached" == NULL);
		}  // initial "try" to overallocate?
	// else:  falling through from the error check + longjmp
	bzb_deref( NULL, stack, barr);
	assert( stack->top == empty_top);

	bza_dest_stack( NULL, &stack);
	}  // _________________________________________________________

/**
 * Helper routine to test byte array concatenation.
 */
static
void					help_test_ba_concat
	(
	t_stack * *			stack,			// stack from which to allocate
	size_t				in_dst			// the byte array to be appended to
	)
	{
	const
	char *				CAT_RESULT = "cha, cha, cha";

	size_t				dst;
	size_t				src;
	size_t				src2;
	int					cnt;
	size_t				result;

	dst = in_dst ?
			in_dst :
			bzb_init_size( NULL, stack, strlen( CAT_RESULT) );
	src = bzb_from_asciiz( NULL, stack, "cha");
	src2 = bzb_from_asciiz( NULL, stack, ", ");
	for ( cnt = 1; ; cnt++)

		{
		result = bzb_concat_to( NULL, stack, dst, src);
		assert( result == dst);  // *not* moved / expanded
		bzb_deref( NULL, *stack, dst);  // code as if no-move not known
		dst = result;
		if ( cnt >= 3)
			{
			break;  // === stop ===
			}  // done?

		result = bzb_concat_to( NULL, stack, dst, src2);
		if ( ! in_dst)
			{
			assert( result == dst);  // *not* moved / expanded
			}  // had to make exact-fit buffer?
		// else:  we were given a small buffer
		bzb_deref( NULL, *stack, dst);  // code as if no-move not known
		dst = result;
		}  // add each "cha" to output

	assert( memcmp( CAT_RESULT,
			bzb_to_asciiz( NULL, *stack, dst),
			bzb_size( NULL, *stack, dst) ) == 0);
	bzb_deref( NULL, *stack, src);
	bzb_deref( NULL, *stack, src2);
	bzb_deref( NULL, *stack, dst);
	}  // _________________________________________________________

/**
 * Test byte array update functionality
 */
static
void					test_mutable_byte_array( void)
	{
/*
	const
	char *				HW = "Hello, world";
	const
	char *				NAME = "Wayne's ";
	const
	char *				SIMPLE_SPLICE = "Hello, Wayne's world";
*/

	t_stack *			stack;
	size_t				empty_top;
	size_t				dst;

	puts( "\nTest mutable byte array use"); fflush( stdout);

	stack = bza_cons_stack( NULL);
	empty_top = stack->top;

	// concatenate to existing largish buffer

	help_test_ba_concat( &stack, 0);
	assert( stack->top == empty_top);

	// concatenate to existing too-small buffer

	dst = bzb_init_size( NULL, &stack, 0);
	help_test_ba_concat( &stack, dst);
	assert( stack->top == empty_top);

	// expand and relocate case:

/*
	dst = bzb_from_asciiz( NULL, &stack, HW);
	src = bzb_from_asciiz( NULL, &stack, NAME);
	result = bzb_splice( NULL, &stack,
			dst, 7, 0,
			src, 0, -1);
	bzb_deref( NULL, stack, dst);
	dst = result;
	assert( memcmp( SIMPLE_SPLICE,
			bzb_to_asciiz( NULL, stack, dst),
			bzb_size( NULL, stack, dst) ) == 0);
	bzb_deref( NULL, stack, dst);
	bzb_deref( NULL, stack, src);
	assert( stack->top == empty_top);
*/

	// reuse large buffer case:

/*
	dst = bzb_init_size( NULL, &stack, strlen( SIMPLE_SPLICE) + 1);

	src = bzb_from_asciiz( NULL, &stack, HW);
	result = bzb_splice( NULL, &stack,
			dst, 0, bzb_size( NULL, stack, src),
			src, 0, -1);
	assert( result == dst);  // *not* moved / expanded
	bzb_deref( NULL, stack, dst);  // code as if no-move not known
	dst = result;
	assert( memcmp( HW,
			bzb_to_asciiz( NULL, stack, dst),
			bzb_size( NULL, stack, dst) ) == 0);
	bzb_deref( NULL, stack, src);

	src = bzb_from_asciiz( NULL, &stack, NAME);
	result = bzb_splice( NULL, &stack,
			dst, 7, 0,
			src, 0, -1);
	assert( result == dst);  // still *not* moved / expanded
	bzb_deref( NULL, stack, dst);  // code as if no-move not known
	dst = result;
	assert( memcmp( SIMPLE_SPLICE,
			bzb_to_asciiz( NULL, stack, dst),
			bzb_size( NULL, stack, dst) ) == 0);
	bzb_deref( NULL, stack, src);
	bzb_deref( NULL, stack, dst);

	assert( stack->top == empty_top);
*/

	// TODO:  test default "-1" in splice args

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
	test_mutable_byte_array();

	// TODO: basic I/O

	// TODO: reference frame management

	// TODO: task messaging

	// TODO: thread as task

	puts( "\n\nNO FATAL ERRORS, ALL GOOD!");

	return 0;
	}  // _________________________________________________________

// vi: ts=4 sw=4 ai
// *** EOF ***
