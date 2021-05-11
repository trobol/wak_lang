#include <wak_lib/file.h>

#ifdef __unix__
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#elif defined(WIN32) || defined(_WIN32)
#include <Windows.h>
#endif



#ifdef __unix__

file_descriptor fd_open(const char* path, int mode) { return open(path, mode); }
int fd_close(file_descriptor fd) { return close(fd); }


int fd_size(file_descriptor fd)
{
	if (fd == -1)
		return 0;
	struct stat sb;
	if (fstat(fd, &sb) == -1)
		return 0;

	return sb.st_size;
}



mmapped_file fd_mem_map(file_descriptor fd) {
	int size = fd_size(fd);
	void *ptr = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0u);
	if (ptr == MAP_FAILED)
		return (mmapped_file){ 0, 0 };

	return (mmapped_file){ (const char*) ptr, size } ;
}


int fd_mem_unmap(mmapped_file* mmf) {
	int err = munmap((void *)mmf->data, mmf->length);
	mmf->data = 0;
	mmf->length = 0;
	return err;
}


#elif defined(WIN32) || defined(_WIN32)

file_descriptor fd_open(const char* path, int mode) { return open(path, mode); }
int fd_close(file_descriptor fd) { return close(fd); }


int fd_size(file_descriptor fd)
{
	if (fd == -1)
		return 0;
	struct stat sb;
	if (fstat(fd, &sb) == -1)
		return 0;

	return sb.st_size;
}



#endif
