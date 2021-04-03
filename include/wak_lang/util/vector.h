#ifndef _WAK_LANG_VECTOR_H
#define _WAK_LANG_VECTOR_H
#include <stdint.h>
#include <stddef.h>


typedef struct vector {
	size_t type_size;
	void* start;
	void* end;
	void* capacity;
} vector;


vector* vector_new(size_t type_size);
vector* vector_new_count(size_t count, size_t type_size);
void vector_free(vector*);

// the number of elements in the vector
size_t vector_count(vector* v);
void vector_append(vector* v, const void* value);
void vector_fill(vector* v, const void* value);

void vector_expand(vector* v, size_t count);



#define define_vector(name, type) \
vector* vector_new_##name() { \
	return vector_new(sizeof(type)); \
} \
vector* vector_new_count_##name(size_t count) { \
	return vector_new_count(count, sizeof(type)); \
} \
void vector_append_##name(vector* v, type val) { \
	return vector_append((void*)&val);\
} 

#endif