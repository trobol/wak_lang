#include <wak_lang/util/vector.h>
#include <wak_lang/util/mem.h>
#include <wak_lang/util/assert.h>

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

size_t vector_count(vector* v) {
	return ((char*)v->end - (char*)v->start) / v->type_size;
}

bool vector_full(vector* v) {
	return (char*)v->capacity >= (char*)v->end;
}

void vector_append(vector* v, const void* val) {
	if (vector_full(v))
		vector_expand(v, 10);
	memcpy(v->end, val, v->type_size);
	v->end = (char*)v->end + v->type_size;
}

void vector_fill(vector* v, const void* val) {
	for (char* itr = (char*)v->start; itr < (char*)v->end; itr+= v->type_size) {
		memcpy(itr, val, v->type_size); 
	}
}

void vector_expand_new_buffer(vector* v, size_t count) {
	size_t new_count = vector_count(v) + count;
	alloc_prevent_overflow(new_count, v->type_size);
	size_t alloc_size = new_count * v->type_size;
	void* ptr = malloc(alloc_size);
	wak_assert_msg(ptr, "failed to expand vector");

	size_t old_size = (char*)v->end - (char*)v->start;
	v->start = ptr;
	v->end = ptr + old_size;
	v->capacity = ptr + alloc_size;
}

void vector_expand(vector* v, size_t count) {
	void *old_start = v->start;
	size_t old_count = (char*)v->end - (char*)v->start;
	vector_expand_new_buffer(v, count);
	memcpy(v->start, old_start, old_count);
}

void vector_insert(vector* v, size_t index) {
	if (vector_full(v)) {
		void *old_start = v->start;
		vector_expand_new_buffer(v, 1);
		size_t offset = index * v->type_size;
		size_t size = (char*)v->end - (char*)v->start;
		memcpy(v->start, old_start, offset);
		memcpy((char*)v->start + offset+1, old_start+offset, size-offset);
	} else {
		char* end = v->start + v->type_size * index; 
		char* dst = v->end;
		char* src;
		do {
			src = dst - v->type_size;
			memcpy(dst, src, v->type_size);
			dst = src;
		} while(dst > end);
	}
	v->end += v->type_size;
}



