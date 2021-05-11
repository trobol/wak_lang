#include <wak_lib/array.h>
#include <wak_lib/mem.h>


void* _array_alloc_impl(size_t count, size_t elem_size) {
	alloc_prevent_overflow(count, elem_size);
	size_t alloc_size = count * elem_size;
	return malloc(alloc_size);
}
