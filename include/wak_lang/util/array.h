#ifndef _WAK_LANG_ARRAY_H
#define _WAK_LANG_ARRAY_H
#include <stdint.h>
#include <stddef.h>

/*
Generic array


Improvements:
memcpy can (probabbly) be optimzied away,
would require some inling or additional "templated" functions

*/

typedef struct {
	size_t type_size;
	void* start;
	void* end;
	void* capacity;
} array;


array* array_new(size_t type_size);
array* array_new_count(size_t count, size_t type_size);
array* array_copy(array* src);
void array_free(array*);

// the number of elements in the array
size_t array_count(array*);
void array_append(array*, const void* value);
void array_fill(array*, const void* value);

void array_expand(array*, size_t count);



#define DEFINE_ARRAY(NAME, TYPE) \
typedef struct { \
	size_t type_size; \
	TYPE* start; \
	TYPE* end; \
	TYPE* capacity; \
} NAME; \
static inline NAME* NAME##_new() { \
	return (NAME*)array_new(sizeof(TYPE)); \
} \
static inline NAME* NAME##_new_count(size_t count) { \
	return (NAME*)array_new_count(count, sizeof(TYPE)); \
} \
static inline void NAME##_append(NAME* v, TYPE val) { \
	array_append((array*)v, (void*)&val);\
}

#endif