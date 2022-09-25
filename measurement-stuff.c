
#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <sys/platform/x86.h>
#include <unistd.h>
#include <pthread.h>
#include <sched.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <errno.h>
#include <time.h>


// adapted from:
// https://github.com/fordsfords/rdtsc/blob/main/rdtsc.h
static inline uint64_t rdtsc() {
	uint32_t be, le;
	// explanation why volatile is necessary:
	// https://stackoverflow.com/a/26456845
	__asm__ volatile ("rdtsc" : "=a" (le), "=d" (be));
	return ((uint64_t) be) << 32 | ((uint64_t) le);
}

struct precise_timer_t {
	uint64_t cpu_hz;
	uint64_t start_ts;
	double overhead_ns;
};


static void calibrate_timer_overhead(struct precise_timer_t *timer);

static void init_timer(struct precise_timer_t *timer, uint64_t cpu_hz) {
	int invariant_tsc = CPU_FEATURE_PRESENT(INVARIANT_TSC);
	if (!invariant_tsc) {
		fprintf(stderr, "CPU does not support invariant TSC, aborting\n");
		exit(1);
	}

	timer->cpu_hz = cpu_hz;
	timer->start_ts = 0;
	timer->overhead_ns = 0.0;

	calibrate_timer_overhead(timer);
}

static inline void start_timer(struct precise_timer_t *timer) {
	timer->start_ts = rdtsc();
}

static double stop_timer_ool(struct precise_timer_t *timer, uint64_t end_ts) {
	if (!timer->start_ts) {
		fprintf(stderr, "stop_timer on non-running timer, aborting\n");
		exit(1);
	}

	uint64_t start_ts = timer->start_ts;
	timer->start_ts = 0;

	return ((double) (end_ts - start_ts)) / ((double) timer->cpu_hz / (double) (1000 * 1000 * 1000)) - timer->overhead_ns;
}

static inline uint64_t stop_timer(struct precise_timer_t *timer) {
	uint64_t end_ts = rdtsc();

	return stop_timer_ool(timer, end_ts);
}

static double denano(double n);

static void calibrate_timer_overhead(struct precise_timer_t *timer) {
	int runs = 100000000;
	printf("calibrating timer overhead with %i runs...\n", runs);

	timer->overhead_ns = 0.0;

	double sum = 0.0;
	for (int i = 0; i < runs; i++) {
		start_timer(timer);
		double elapsed = stop_timer(timer);
		sum += elapsed;
	}

	timer->overhead_ns = sum / (double) runs;

	printf("calibration complete, took %f s, timer overhead estimated at %f ns\n", denano(sum), timer->overhead_ns);
}


uint64_t mega(uint64_t n) { return n * 1000 * 1000; }

double denano(double n) { return n / (1000 * 1000 * 1000); };


static inline void op() {
	sleep(1);
}


void pin_cpu(void) {
	printf("pinning thread to CPU\n");
	// pin to process to CPU
	// 
	// in addition to avoiding benchmark noise from process migration,
	// allows us to straightforwardly detect time using `rdtsc`
	// rather than having to use `rdtscp` and estimating relative skew
	// between cores and all that
	pthread_t my_thread = pthread_self();
	int my_cpu = sched_getcpu();

	cpu_set_t cpu_set;
	CPU_ZERO(&cpu_set);
	CPU_SET(my_cpu, &cpu_set);

	int e = pthread_setaffinity_np(my_thread, sizeof(cpu_set_t), &cpu_set);
	if (e) {
		fprintf(stderr, "error %i from pthread_setaffinity_np\n", e);
		exit(1);
	}
}

void maximize_priority(void) {
	printf("maximizing process priority\n");
	// maximize process priority to minimize chance of other processes preempting us
	// adding noise to benchmarks
	//
	// see man page for why we need to be using errno as such here
	for (int i = -20; i < 0; i++) {
		printf("attempting to set priority to %i\n", i);
		errno = 0;
		setpriority(PRIO_PROCESS, 0, i);
		if (errno) {
			fprintf(stderr, "error %i from setpriority\n", errno);
			if (errno == EACCES) {
				fprintf(stderr, "error = 'permission denied'\n");
			}
		} else {
			break;
		}
	}
}

void prep_proc_for_benchmark(void) {
	pin_cpu();
	//maximize_priority();
}


double timespec_to_ns(struct timespec *t) {
	return (double) t->tv_sec * 1000 * 1000 * 1000 + (double) t->tv_nsec;
}





int main() {
	prep_proc_for_benchmark();

	/*
	struct timespec res;
	clock_getres(CLOCK_MONOTONIC, &res);
	printf("clock res = %f ns\n", timespec_to_ns(&res));
	*/

	struct precise_timer_t timer;
	init_timer(&timer, mega(2700));
		
	/*
	uint64_t before = rdtsc();
	op();
	uint64_t after = rdtsc();

	uint64_t delta = after - before;
	printf("delta=%lu\n", delta);
	*/

	start_timer(&timer);
	op();
	double elapsed = stop_timer(&timer);

	printf("estimating elapsed %f s\n", denano(elapsed));	
}

