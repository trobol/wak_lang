#include <wak_lang/util/array.h>
#include <wak_lang/util/mem.h>

#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>


array* array_new(size_t type_size) {
	array* v = malloc(sizeof(array));
	*v = (array){type_size, 0, 0, 0};
	v->type_size = type_size;
	return v;
}

array* array_new_count(size_t count, size_t type_size) {
	array* v = array_new(type_size);
	array_expand(v, count);
	v->end = v->capacity;
	return v;
}

void array_free(array* v) {
	free(v->start);
	free(v);
}

size_t array_count(array* v) {
	return ((char*)v->end - (char*)v->start) / v->type_size;
}

void array_append(array* v, const void* val) {
	if ((char*)v->capacity >= (char*)v->end)
		array_expand(v, 10);
	memcpy(v->end, val, v->type_size);
	v->end = (char*)v->end + v->type_size;
}

void array_fill(array*  v, const void* val) {
	for (char* itr = (char*)v->start; itr < (char*)v->end; itr+= v->type_size) {
		memcpy(itr, val, v->type_size); 
	}
}


void array_expand(array* v, size_t count) {
	size_t new_count = array_count(v) + count;
	alloc_prevent_overflow(new_count, v->type_size);
	size_t alloc_size = new_count * v->type_size;
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


