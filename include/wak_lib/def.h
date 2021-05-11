#ifndef _WAK_LIB_DEF_H
#define _WAK_LIB_DEF_H


#if defined(__GNUC__) || defined(__clang__)
#define WAK_UNLIKELY(v) __builtin_expect((v), 0)
#define WAK_LIKELY(v) __builtin_expect((v), 1)
#else
#define WAK_UNLIKELY(v) (v)
#define WAK_LIKELY(v) (v)
#endif


#if defined(__GNUC__)
#define WAK_FORCEINLINE __inline__ __attribute__((always_inline))
#define WAK_NOINLINE __attribute__((noinline))
#define WAK_NORETURN __attribute__((__noreturn__))
#elif defined(_MSC_VER)
#define WAK_NOINLINE
#define WAK_FORCEINLINE
#define WAK_NORETURN __declspec(noreturn)
#else
#define WAK_FORCEINLINE
#define WAK_NOINLINE
#define WAK_NORETURN
#endif

#if defined(__clang__) && __has_attribute(musttail)
#define WAK_MUSTTAIL __attribute__((musttail))
#else
#define WAK_MUSTTAIL
#endif


#endif