#include <wak_lib/timers.h>
#include <wak_lib/assert.h>

#include <time.h>


const char* TIMER_NAMES[TIMER_ID_COUNT] = {
	"TOTAL",
	"FILE_READ",
	"LEXETIZE",
	"PARSE",
	"SERIALIZE",
	"FILE_WRITE"
};


timespec start_times[TIMER_ID_COUNT];
timespec end_times[TIMER_ID_COUNT];


double ms_duration(timespec start, timespec end) {
	long total_sec = end.tv_sec - start.tv_sec;
	long total_nsec = end.tv_nsec - start.tv_nsec;

	long sec_ms = total_sec * 1000;
	double nsec_ms = (double)total_nsec / (double)1000000;
	return (double)sec_ms + nsec_ms;
}


void timer_start(Timer_ID id) {
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, start_times + id);
}

void timer_end(Timer_ID id) {
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, end_times + id);
}

void timer_report(Timer_ID id) {
	printf("%15s: ", TIMER_NAMES[id]);

	if (start_times[id].tv_nsec == 0)
		printf("%12s\n","not started");
	else if (end_times[id].tv_nsec == 0)
		printf("%12s\n","not ended");
	else
		printf("%10.5fms\n", ms_duration(start_times[id], end_times[id]));
}

void timer_report_all() {
	timespec res;
	clock_getres(CLOCK_PROCESS_CPUTIME_ID, &res);

	printf("\n\nTimer report: (resolution %ldns)\n", res.tv_nsec);
	for(int i = 0; i < TIMER_ID_COUNT; i++)
		timer_report((Timer_ID)i);
}