#include <wak_lang/util/timers.h>
#include <wak_lang/util/assert.h>

#include <time.h>


const char* TIMER_NAMES[TIMER_ID_COUNT] = {
	"TOTAL",
	"FILE_READ",
	"LEXETIZE",
	"PARSE",
	"SERIALIZE",
	"FILE_WRITE"
};

long start_times[TIMER_ID_COUNT];
long end_times[TIMER_ID_COUNT];

typedef struct timespec timespec;

long get_time_ns() {
	timespec time;
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time);
	return time.tv_nsec;
}

void timer_start(Timer_ID id) {
	
	start_times[id] = get_time_ns();
}

void timer_end(Timer_ID id) {
	end_times[id] = get_time_ns();
}

#define NS_TO_MS 1000000

void timer_report(Timer_ID id) {
	printf("%15s: ", TIMER_NAMES[id]);
	long total = end_times[id] - start_times[id];

	if (start_times[id] == 0)
		printf("%12s\n","not started");
	else if (end_times[id] == 0)
		printf("%12s\n","not ended");
	else
		printf("%10.5fms\n", (double) total / NS_TO_MS);
}

void timer_report_all() {
	timespec res;
	clock_getres(CLOCK_PROCESS_CPUTIME_ID, &res);

	printf("\n\nTimer report: (resolution %ldns)\n", res.tv_nsec);
	for(int i = 0; i < TIMER_ID_COUNT; i++)
		timer_report((Timer_ID)i);
}