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

clock_t start_times[TIMER_ID_COUNT];
clock_t end_times[TIMER_ID_COUNT];

void timer_start(Timer_ID id) {
	start_times[id] = clock();
}

void timer_end(Timer_ID id) {
	end_times[id] = clock();
}

void timer_report(Timer_ID id) {
	printf("%15s: ", TIMER_NAMES[id]);
	clock_t total = end_times[id] - start_times[id];

	if (start_times[id] == 0)
		printf("not started\n");
	else if (end_times[id] == 0)
		printf("not ended\n");
	else
		printf("%f\n", (double) total / CLOCKS_PER_SEC);
}

void timer_report_all() {
	printf("\n\nTimer report:\n");
	for(int i = 0; i < TIMER_ID_COUNT; i++)
		timer_report((Timer_ID)i);
}