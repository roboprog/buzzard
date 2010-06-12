/**
 * Test harness for runtime library.
 */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "bzrt_alloc.h"

/**
 * Test (very basic) stack creation / destruction.
 */
static
void					test_stack_init( void)
	{
	t_stack *			stack;

	puts( "Test basic stack creation");

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

	puts( "Test basic stack frame allocation and deallocation");

	stack = bza_cons_stack();

	tops[ 0 ] = stack->top;

	// throw a few blocks on the stack:

	frames[ 0 ] = bza_cons_stk_frame( &stack, 64);
	tops[ 1 ] = stack->top;
	assert( ( tops[ 0 ] + 64) == tops[ 1 ]);
	assert( tops[ 0 ] == frames[ 0 ]);

	frames[ 1 ] = bza_cons_stk_frame( &stack, 128);
	tops[ 2 ] = stack->top;
	assert( ( tops[ 1 ] + 128) == tops[ 2 ]);
	assert( tops[ 1 ] == frames[ 1 ]);

	frames[ 2 ] = bza_cons_stk_frame( &stack, 32);
	tops[ 3 ] = stack->top;
	assert( ( tops[ 2 ] + 32) == tops[ 3 ]);
	assert( tops[ 2 ] == frames[ 2 ]);

	// play with frame release, including out-of-order release

	bza_deref_stk_frame( stack, frames[ 2 ]);
	assert( stack->top == tops[ 2 ]);

	bza_deref_stk_frame( stack, frames[ 0 ]);
	assert( stack->top == tops[ 2 ]);  // since frames[ 1 ] is still allocated

	bza_deref_stk_frame( stack, frames[ 1 ]);
	assert( stack->top == tops[ 0 ]);  // double pop

	bza_dest_stack( &stack);
	}  // _________________________________________________________

/**
 * Drive tests.
 * TODO: xunit or something like that (but exit-on-failure for now)
 */
int						main
	(
	char *				argv []
	)
	{
	test_stack_init();
	test_stack_alloc();
	// TODO:  play with reference counts
	return 0;
	}  // _________________________________________________________

// vi: ts=4 sw=4 ai
// *** EOF ***
