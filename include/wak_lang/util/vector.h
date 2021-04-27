#ifndef _WAK_LANG_VECTOR_H
#define _WAK_LANG_VECTOR_H
#include <stdint.h>
#include <stddef.h>

/*
Generic vector


Improvements:
memcpy can (probably) be optimized away,
would require some inling or additional "templated" functions

*/

typedef struct {
	size_t type_size;
	void* start;
	void* end;
	void* capacity;
} vector;


vector* vector_new(size_t type_size);
vector* vector_new_count(size_t count, size_t type_size);
vector* vector_copy(vector* src);
void vector_free(vector*);

// the number of elements in the vector
size_t vector_count(vector*);
void vector_append(vector*, const void* value);
void vector_fill(vector*, const void* value);

void vector_expand(vector*, size_t count);



#define DEFINE_VECTOR(NAME, TYPE) \
typedef struct { \
	size_t type_size; \
	TYPE* start; \
	TYPE* end; \
	TYPE* capacity; \
} NAME; \
static inline NAME* NAME##_new() { \
	return (NAME*)vector_new(sizeof(TYPE)); \
} \
static inline NAME* NAME##_new_count(size_t count) { \
	return (NAME*)vector_new_count(count, sizeof(TYPE)); \
} \
static inline void NAME##_append(NAME* v, TYPE val) { \
	vector_append((vector*)v, (void*)&val);\
}

#endif