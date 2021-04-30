#include <wak_lang/util/assert.h>
#include <wak_lang/util/timers.h>
#include <wak_lang/util/file.h>

#include <wak_lang/lexer.h>
#include <wak_lang/parser.h>
#include <float.h>

#define ITERATIONS 100

int main(int argc, char *argv[]) {

	double min = DBL_MAX;
	double max = 0;
	double first_time = 0;
	long double total = 0;

	const char* filepath = "../long_example_file.txt";


	file_descriptor src_fd = fd_open(filepath, 0);
	if (src_fd == -1) {
		printf("could not open file\n");
		return 1;
	}
	mmapped_file src_mmf = fd_mem_map(src_fd);

	for (uint32_t i = 0; i < ITERATIONS; i++) {
		timespec start, end;

		clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
		Token_Module module = lexetize(src_mmf.data, src_mmf.data+src_mmf.length+1);
		clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);

		token_module_free(&module);

		double time = ms_duration(start, end);
		min = time < min ? time : min;
		max = time > max ? time : max;
		total += time;

		if (i == 0) first_time = time;

		printf("%d, %fms\n", i, time);
	}

	double average = total / (long double)ITERATIONS;

	printf("iterations: %dms\n", ITERATIONS);
	printf("total: %Lfms\n", total);
	printf("min: %fms\n", min);
	printf("max: %fms\n", max);
	printf("average: %fms\n", average);
	printf("first: %fms\n", first_time);

	// free file resources
	fd_mem_unmap(&src_mmf);
	fd_close(src_fd);


	return 0;
}