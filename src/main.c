#include <wak_lang/assert.h>
#include <wak_lang/timers.h>
#include <time.h>

int main() {
	timer_start(TIMER_TOTAL);
	




	timer_end(TIMER_TOTAL);
	timer_report_all();
	return 0;
}