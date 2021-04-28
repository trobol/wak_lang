#ifndef _WAK_LANG_ASSERT_ASSERT_H
#define _WAK_LANG_ASSERT_ASSERT_H
#include <stdio.h>

#ifdef NDEBUG
#define wak_assert(v)
#define wak_assert_msg(v, msg)
#else
#include "_debug_break.h"
static inline void _wak_assert_fail(const char* expr, const char* msg, const char* file, int line) {
	printf("%s ( %s ) at: %s:%d\n", msg, expr, file, line);
	fflush(stdout); 
	debug_break();
} 
#define wak_assert(v) \
	if (!(v)) _wak_assert_fail(#v, "assertion failed", __FILE__, __LINE__);
#define wak_assert_msg(v, msg) \
	if (!(v)) _wak_assert_fail(#v, msg, __FILE__, __LINE__);
#endif

#endif