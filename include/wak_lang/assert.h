#ifndef _WAK_LANG_ASSERT_ASSERT_H
#define _WAK_LANG_ASSERT_ASSERT_H
#include <stdio.h>

#ifdef NDEBUG
#define _wak_assert(v)
#else
static void _wak_assert_fail(const char* expr, const char* file, int line) {
	printf("assertion failed ( %s ) at: %s:%d\n", expr, file, line);
	fflush(stdout);
}
#include "debug_break.h"
#define _wak_assert(v) \
	if (!(v)) {\
		_wak_assert_fail(#v, __FILE__, __LINE__);\
		debug_break();\
	}
#endif

#endif