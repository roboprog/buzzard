/**
 * Simple macros for ad-hoc trace logging of library internals
 */


#ifdef DO_LOG
	#define MLOG_PUTS( s) fputs( (s), stderr)

	// TODO: something a bit more clever (var-arg macro?)
	#define MLOG_PRINTF	fprintf
#else
	#define MLOG_PUTS( s) /* */

	#define MLOG_PRINTF	//
#endif  // DO_LOG defined?

// *** EOF ***
