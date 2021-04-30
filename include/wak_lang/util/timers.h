#ifndef _WAK_LANG_TIMERS_H
#define _WAK_LANG_TIMERS_H
#include <time.h>

typedef enum {
	TIMER_TOTAL,
	TIMER_FILE_READ,
	TIMER_LEXETIZE,
	TIMER_PARSE,
	TIMER_SERIALIZE,
	TIMER_FILE_WRITE,
	TIMER_ID_COUNT
} Timer_ID;

typedef struct timespec timespec;

void timer_start(Timer_ID);
void timer_end(Timer_ID);
void timer_report(Timer_ID);
void timer_report_all();

double ms_duration(timespec start, timespec end);

#endif