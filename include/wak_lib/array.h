#ifndef _WAK_LIB_ARRAY_H
#define _WAK_LIB_ARRAY_H
#include <stddef.h>


#define array_alloc(COUNT, TYPE) (TYPE*)_array_alloc_impl(COUNT, sizeof(TYPE))
void* _array_alloc_impl(size_t count, size_t type_size_size);

#define array_realloc(PTR, COUNT, TYPE) (TYPE*)_array_realloc_impl(PTR, COUNT, sizeof(TYPE))
void* _array_realloc_impl(void* ptr, size_t count, size_t type_size);

#endif