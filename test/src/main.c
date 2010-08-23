/**
 * Test harness for runtime library.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "bzrt_alloc.h"

/**
 * Test (very basic) stack creation / destruction.
 */
static
void					test_stack_init( void)
	{
	t_stack *			stack;

	puts( "Test basic stack creation"); fflush( stdout);

	stack = bza_cons_stack();
	assert( stack != NULL);

	bza_dest_stack( &stack);
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

	puts( "Test basic stack frame allocation and deallocation"); fflush( stdout);

	stack = bza_cons_stack();

	tops[ 0 ] = stack->top;

	// throw a few blocks on the stack:

	// puts( "1");  // TEMP
	frames[ 0 ] = bza_cons_stk_frame( &stack, 64);
	memset( bza_get_frame_ptr( stack, frames[ 0 ]), 'X', 64);
	tops[ 1 ] = stack->top;
	// assert( ( tops[ 0 ] + 64) <= tops[ 1 ]);
	assert( ( tops[ 0 ] <= frames[ 0 ]) &&
			( frames[ 0 ] <= tops[ 1 ]) );

	// puts( "2");  // TEMP
	frames[ 1 ] = bza_cons_stk_frame( &stack, 128);
	memset( bza_get_frame_ptr( stack, frames[ 1 ]), 'Y', 128);
	tops[ 2 ] = stack->top;
	// assert( ( tops[ 1 ] + 128) <= tops[ 2 ]);
	assert( ( tops[ 1 ] <= frames[ 1 ]) &&
			( frames[ 1 ] <= tops[ 2 ]) );

	// puts( "3");  // TEMP
	frames[ 2 ] = bza_cons_stk_frame( &stack, 32);
	memset( bza_get_frame_ptr( stack, frames[ 2 ]), 'Z', 32);
	tops[ 3 ] = stack->top;
	// assert( ( tops[ 2 ] + 32) <= tops[ 3 ]);
	assert( ( tops[ 2 ] <= frames[ 2 ]) &&
			( frames[ 2 ] <= tops[ 3 ]) );

	// play with frame release, including out-of-order release
	bza_ref_stk_frame( stack, frames[ 1 ]);  // extra count on frame 1!

	// puts( "4");  // TEMP
	bza_deref_stk_frame( stack, frames[ 2 ]);
	assert( stack->top == tops[ 2 ]);

	// puts( "5");  // TEMP
	bza_deref_stk_frame( stack, frames[ 1 ]);
	assert( stack->top == tops[ 2 ]);  // since frames[ 1 ] is still allocated

	// puts( "6");  // TEMP
	bza_deref_stk_frame( stack, frames[ 0 ]);
	assert( stack->top == tops[ 2 ]);  // since frames[ 1 ] is still allocated
	bza_deref_stk_frame( stack, frames[ 1 ]);  // get rid of "extra" count
	assert( stack->top == tops[ 0 ]);  // double pop

	bza_dest_stack( &stack);
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
	// TODO:  play with reference counts
	// TODO:  play with out of memory error handling
	return 0;
	}  // _________________________________________________________

// vi: ts=4 sw=4 ai
// *** EOF ***
