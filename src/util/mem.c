#include <wak_lang/util/mem.h>

#include <errno.h>

// if alloc size overflows size_t,
// not enough memory will be allocated
// and we might read from memory we shouldn't
int alloc_will_overflow(size_t count, size_t elem_size) {
	return (count && elem_size && SIZE_MAX / count < elem_size);
}

void alloc_prevent_overflow(size_t count, size_t elem_size) {
	if(alloc_will_overflow(count, elem_size)) {
		exit(ENOMEM);
	}
}

void* alloc_array(size_t count, size_t elem_size) {
	alloc_prevent_overflow(count, elem_size);
	size_t alloc_size = count * elem_size;
	return malloc(alloc_size);
}