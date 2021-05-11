#ifndef _WAK_LIB_FILE_H
#define _WAK_LIB_FILE_H
#include <stddef.h>

// FILE DESTRIPTORS
typedef int file_descriptor;


file_descriptor fd_open(const char* path, int mode);
int fd_close(file_descriptor fd);

int fd_size();

// Memory Mapped File

typedef struct mmapped_file {
	const char* data;
	size_t length;
} mmapped_file;

mmapped_file fd_mem_map(file_descriptor fd);
int fd_mem_unmap(mmapped_file* mmf);

#endif