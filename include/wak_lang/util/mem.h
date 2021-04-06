#ifndef _WAK_LANG_MEM_H
#define _WAK_LANG_MEM_H
#include <stddef.h>
#include <stdint.h>

/*
 * functions to help with safe memory management
*/



#define alloc_array(COUNT, TYPE) (TYPE*)alloc_array_impl(COUNT, sizeof(TYPE))
/*
 * allocates count * elem_size bytes
 * after checking that it does not overflow size_t
*/
void* alloc_array_impl(size_t count, size_t elem_size);

/*
 * force crash if count * elem_size will overflow
*/
void alloc_prevent_overflow(size_t count, size_t elem_size);

/*
 * will count * elem_size overflow size_t
*/
int alloc_will_overflow(size_t count, size_t elem_size);


void* malloc(size_t size);

/*
 * alloc and zero pad, size is count+1
*/
char* malloc_str(size_t count);

#endif