#include <wak_lang/util/assert.h>
#include <wak_lang/util/timers.h>
#include <wak_lang/util/file.h>

#include <time.h>

int main(int argc, char *argv[]) {
	//_wak_assert_msg(0, "test");
	timer_start(TIMER_TOTAL);

	if (argc != 2) {
		printf("invalid number of arguments\n");
		return 1;
	}

	timer_start(TIMER_FILE_READ);
	file_descriptor src_fd = fd_open(argv[1], 0);
	if (!src_fd) {
		printf("could not open file\n");
		return 1;
	}
	mmapped_file src_mmf = fd_mem_map(src_fd);
	timer_end(TIMER_FILE_READ);


	timer_start(TIMER_LEXETIZE);

	

	// free file resources
	fd_mem_unmap(&src_mmf);
	fd_close(src_fd);
	timer_end(TIMER_LEXETIZE);



	timer_end(TIMER_TOTAL);
	timer_report_all();
	return 0;
}