#include <wak_lib/mem.h>
#include <stdlib.h>

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

char* malloc_str(size_t count) {
	size_t size = count+1;
	char* str = malloc(size);
	str[count] = '\0';
	return str;
}