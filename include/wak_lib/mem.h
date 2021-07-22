#ifndef _WAK_LIB_MEM_H
#define _WAK_LIB_MEM_H
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include <wak_lib/def.h>

/*
 * functions to help with safe memory management
*/



// if alloc size overflows size_t,
// not enough memory will be allocated
// and we might read from memory we shouldn't
static inline int alloc_will_overflow(size_t count, size_t elem_size) {
	return (count && elem_size && SIZE_MAX / count < elem_size);
}

/*
 * force crash if count * elem_size will overflow
*/
static inline void alloc_prevent_overflow(size_t count, size_t elem_size) {
	if(WAK_UNLIKELY(alloc_will_overflow(count, elem_size))) {
		exit(ENOMEM);
	}
}

/*
 * alloc and zero pad, size is count+1
*/
static inline char* malloc_str(size_t count) {
	size_t size = count+1;
	char* str = malloc(size);
	str[count] = '\0';
	return str;
}


/*
 * alloc count * elem_size bytes, crash if alloc fails
 */
static inline void* _array_alloc_impl(size_t count, size_t elem_size) {
	alloc_prevent_overflow(count, elem_size);
	size_t alloc_size = count * elem_size;
	void* ptr = malloc(alloc_size);
	if (WAK_UNLIKELY(ptr == 0)) exit(ENOMEM);
	return ptr;
}

/*
 * realloc count * elem_size bytes, crash if realloc fails
 */
static inline void* _array_realloc_impl(void* ptr, size_t count, size_t elem_size) {
	alloc_prevent_overflow(count, elem_size);
	size_t alloc_size = count * elem_size;
	void* out = realloc(ptr, alloc_size);
	if (WAK_UNLIKELY(out == 0)) exit(ENOMEM);
	return out;
}


/*
 * alloc an array with {COUNT} elements of type {TYPE}
 * will exit if the total size would overflow uint64 or if alloc fails
 */
#define array_alloc(COUNT, TYPE) (TYPE*)_array_alloc_impl(COUNT, sizeof(TYPE))

/*
 * realloc an array with {COUNT} elements of type {TYPE}
 * will exit if the total size would overflow uint64 or if realloc fails
 */
#define array_realloc(PTR, COUNT, TYPE) (TYPE*)_array_realloc_impl(PTR, COUNT, sizeof(TYPE))


#endif