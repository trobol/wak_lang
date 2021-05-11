#ifndef _WAK_LIB_MEM_H
#define _WAK_LIB_MEM_H
#include <stddef.h>
#include <stdint.h>

/*
 * functions to help with safe memory management
*/


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