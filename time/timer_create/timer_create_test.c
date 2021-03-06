/*
 *
 */
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <time.h> /* clockid_t timer_t 'struct itimerspec' */
#include <signal.h>
#include <unistd.h>

typedef enum {false = 0, true = 1} bool;
#define ATF_REQUIRE(condition)                         \
do {                                                                            \
	if (!(condition)) {                                               \
		printf("Error %s\n", strerror(errno));   \
	}                                                                          \
} while(0)

static timer_t t;
static bool fail = true;
static void timer_signal_handler(int signo, siginfo_t *si, void *osi)
{
	timer_t *tp;
	
	(void)osi;
	tp = si->si_value.sival_ptr;
	if (*tp == t && signo == SIGALRM) {
		fail = false;
	}

	(void)fprintf(stderr, "%s: %s \n", __func__, strsignal(signo));
}
int main(int argc, char **argv)
{
	struct itimerspec tim = {{ 0 }, }; /* 使用0值初始化,要比memset效率高些 */
	struct sigaction act = {{ 0 }, };
	struct sigevent evt = {{ 0 }, };
	sigset_t set;

	t = 0;
	fail = true;

	/*
	  * Set handler
	  */
	act.sa_flags = SA_SIGINFO;
	act.sa_sigaction = timer_signal_handler;

	ATF_REQUIRE(sigemptyset(&set) == 0);
	ATF_REQUIRE(sigemptyset(&act.sa_mask) == 0);

	/*
	  * Block SIGALRM while configuring the timer
	  */
	ATF_REQUIRE(sigaction(SIGALRM, &act, NULL) == 0);
	ATF_REQUIRE(sigaddset(&set, SIGALRM) == 0);
	ATF_REQUIRE(sigprocmask(SIG_SETMASK, &set, NULL) == 0);

	/*
	  * Create the timer (SIGEV_SIGNAL)
	  */
	evt.sigev_signo = SIGALRM;
	evt.sigev_value.sival_ptr = &t;
	evt.sigev_notify = SIGEV_SIGNAL;

	ATF_REQUIRE(timer_create(CLOCK_REALTIME, &evt, &t) == 0);

	/*
	  * Start the timer. After this, unblock the signal.
	  */
	tim.it_value.tv_sec = 1;
	tim.it_value.tv_nsec = 0;
	tim.it_interval.tv_sec = 3; /* tim.it_interval表示循环timer */
	tim.it_interval.tv_nsec = 0;

	ATF_REQUIRE(timer_settime(t, 0, &tim, NULL) == 0);

	(void)sigprocmask(SIG_UNBLOCK, &set, NULL);
	(void)sleep(2);

	ATF_REQUIRE(timer_delete(t) == 0);

	return 0;
}
