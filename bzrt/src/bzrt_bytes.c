/**
 * Byte array primitives for buzzard.
 *
 * $Id: $
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

#include "bzrt_bytes.h"

#define DO_LOG	1

#ifdef DO_LOG
	#define MLOG_PUTS( s) fputs( (s), stderr)

	// TODO: something a bit more clever (var-arg macro?)
	#define MLOG_PRINTF	fprintf
#else
	#define MLOG_PUTS( s) /* */

	#define MLOG_PRINTF	//
#endif  // DO_LOG defined?


// TODO:  some code


// vi: ts=4 sw=4 ai
// *** EOF ***
