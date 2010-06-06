/**
 * Test harness for runtime library.
 */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "bzrt_alloc.h"

static
void					test_stack_init( void)
	{
	t_stack *			stack;

	stack = bza_cons_stack();
	assert( stack != NULL);

	bza_dest_stack( &stack);
	assert( stack == NULL);
	}  // _________________________________________________________

int						main
	(
	char *				argv []
	)
	{
	puts( "Test basic stack creation");
	test_stack_init();
	return 0;
	}  // _________________________________________________________

// vi: ts=4 sw=4 ai
// *** EOF ***
