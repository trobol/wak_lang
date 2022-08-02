#include <wak_lib/assert.h>
#include <wak_lib/timers.h>
#include <wak_lib/file.h>

#include <wak_lang/lexer.h>
#include <wak_lang/parser.h>
#include <float.h>

#include <errno.h>

#include <coz.h>

int main(int argc, char *argv[]) {

	if (argc != 2) {
		printf("error: missing iteration count\n");
		return 1;
	}

	int iterations = atoi(argv[1]);

	double min = DBL_MAX;
	double max = 0;
	double first_time = 0;
	double total = 0;



	const char* filepath = "./long_example_file.txt";


	file_descriptor src_fd = fd_open(filepath, 0);
	if (src_fd == -1) {
		printf("error: could not open file\n");
		printf("Error description is : %s\n",strerror(errno));
		return 1;
	}
	mmapped_file src_mmf = fd_mem_map(src_fd);

	for (int i = 0; i < iterations; i++) {
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

		printf("%d: %fms\n", i, time);
	}

	double average = total / (double)iterations;

	double size_kb = src_mmf.length / 1000.0;
	double min_mbps = size_kb / min;
	double max_mbps = size_kb / max;
	double average_mbps = size_kb / average;
	double first_mbps = size_kb / first_time;

	printf("\nRESULTS:\n");
	printf("iterations: %d\n", iterations);
	printf("total:   %9.4fms\n", total);
	printf("min: 	 %9.4fms (%4.1fmbps)\n", min, min_mbps );
	printf("max:     %9.4fms (%4.1fmbps)\n", max, max_mbps);
	printf("average: %9.4fms (%4.1fmbps)\n", average, average_mbps);
	printf("first:   %9.4fms (%4.1fmbps)\n", first_time, first_mbps);

	// free file resources
	fd_mem_unmap(&src_mmf);
	fd_close(src_fd);


	return 0;
}