#include <wak_lang/util/vector.h>
#include <wak_lang/util/mem.h>

#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>


vector* vector_new(size_t type_size) {
	vector* v = malloc(sizeof(vector));
	*v = (vector){type_size, 0, 0, 0};
	v->type_size = type_size;
	return v;
}

vector* vector_new_count(size_t count, size_t type_size) {
	vector* v = vector_new(type_size);
	vector_expand(v, count);
	v->end = v->capacity;
	return v;
}

void vector_free(vector* v) {
	free(v->start);
	free(v);
}

// the number of elements in the vector
size_t vector_count(vector* v) {
	return ((char*)v->end - (char*)v->start) / v->type_size;
}

void vector_append(vector* v, const void* val) {
	if ((char*)v->capacity >= (char*)v->end)
		vector_expand(v, 10);
	memcpy(v->end, val, v->type_size);
}

void vector_fill(vector* v, const void* val) {
	for (char* itr = (char*)v->start; itr < (char*)v->end; itr+= v->type_size) {
		memcpy(itr, val, v->type_size); 
	}
}




void vector_expand(vector* v, size_t count) {
	alloc_prevent_overflow(count, v->type_size);
	size_t alloc_size = count * v->type_size;
	void* ptr;
	if (v->start == NULL) {
		ptr = malloc(alloc_size);
	} else {
		ptr = realloc(v->start, alloc_size);
	}
	size_t old_size = (char*)v->end - (char*)v->start;
	v->start = ptr;
	v->end = ptr + old_size;
	v->capacity = ptr + alloc_size;
}


