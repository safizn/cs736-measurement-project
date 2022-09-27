
#include "rdtsc_timer.h"
#include "trusted_timer.h"
#include <stdlib.h>
#include <stdio.h>
#include <sys/platform/x86.h>
#include <sys/resource.h>
#include <x86intrin.h>

/**
 * Get the current CPU's current timestamp counter value using the rdtsc
 * instruction.
 *
 * adapted from:
 * https://github.com/fordsfords/rdtsc/blob/main/rdtsc.h
 */
/*
uint64_t rdtsc() {
*/	/*
    uint32_t be, le;
    // explanation why volatile is necessary:
    // https://stackoverflow.com/a/26456845
    __asm__ volatile ("rdtsc" : "=a" (le), "=d" (be));
    return ((uint64_t) be) << 32 | ((uint64_t) le);
	*//*
	//return __rdtsc();
}
*/

uint64_t rdtsc_before() {
	uint32_t hi, lo;
	__asm__ __volatile__ ("CPUID\n\t"
		"RDTSC\n\t"
		"mov %%edx, %0\n\t"
		"mov %%eax, %1\n\t": "=r" (hi), "=r" (lo)::"%rax", "%rbx", "%rcx", "%rdx", "memory");
	return ((uint64_t) hi) << 32 | ((uint64_t) lo);
}

uint64_t rdtsc_after() {
	uint32_t hi, lo;
	__asm__ __volatile__ ("RDTSCP\n\t"
		"mov %%edx, %0\n\t"
		"mov %%eax, %1\n\t"
		"CPUID\n\t": "=r" (hi), "=r" (lo)::"%rax", "%rbx", "%rcx", "%rdx", "memory");
	return ((uint64_t) hi) << 32 | ((uint64_t) lo);
}

inline void start_rdtsc_timer(struct rdtsc_timer *timer) {
    timer->start = rdtsc_before();
}

/**
 * Part of `stop_timer` implementation that doesn't need to be inlined.
 */
double stop_rdtsc_timer_ool(struct rdtsc_timer *timer, uint64_t end);

inline double stop_rdtsc_timer(struct rdtsc_timer *timer) {
    uint64_t end = rdtsc_after();
    stop_rdtsc_timer_ool(timer, end);
}

double stop_rdtsc_timer_ool(struct rdtsc_timer *timer, uint64_t end) {
    // assert actually is running
    if (!timer->start) {
        fprintf(stderr, "stop_rdtsc_timer on non-running timer, aborting\n");
        exit(1);
    }

    // return timer to non-running state
    uint64_t start = timer->start;
    //timer->start = 0;

    // do the following floating point arithmetic in a strategic order to avoid
    // loss of significant bits assuming that the time measured is a relatively
    // small number of nanoseconds
    return
        // divide "number of timestamps" by "timestamps per nanosecond"
        // producing "number of nanoseconds".
        //
        // since these are, for small measurements, both within a few orders of
        // of magnitude of 1, meaning that they're relatively close to each
        // other in magnitude, that should minimize loss of significant digits
        // due to floating point rounding.
        (double) (
            // compute number of timestamps, producing a value within a few
            // orders of magnitude of 1 for tiny measurements
            end - start
        )
	* timer->ns_per_tsc
        /*/ (
            // convert from timestamps per second to timestamps per nanosecond,
            // producing a value within a few order of magnitudes of 1
            (double) timer->cpu_hz
            / (double) (1000 * 1000 * 1000)
        )*/
        // subtract the estimated overhead.
        - timer->overhead;
}

/**
 * Procedurally estimate and initialize an rdtsc timer's `overhead` value.
 */
void calibrate_rdtsc_timer(struct rdtsc_timer *timer) {
    int runs = 10000000;
    printf("calibrating timer overhead with %i runs...\n", runs);

    // we will run the calibration with overhead 0
    timer->overhead = 0.0;

    // simply compute the average nanoseconds it measures when you start the
    // timer then stop it with no additional instructions in between.
    double sum = 0.0;
    for (int i = 0; i < runs; i++) {
        start_rdtsc_timer(timer);
        double elapsed = stop_rdtsc_timer(timer);
        sum += elapsed;
    }

    timer->overhead = sum / (double) runs;

    printf(
        "calibration complete, took %f s, timer overhead estimated at %f ns\n",
        sum / (1000 * 1000 * 1000), timer->overhead
    );
}

static void measure_ns_per_tsc_datapoint(
		struct trusted_timer *timer,
		double measure_for_ns,
		double *delta_ns,
		uint64_t *delta_tsc
) {
	uint64_t start_tsc = rdtsc_before();
	start_trusted_timer(timer);

	do {
		*delta_ns = stop_trusted_timer(timer);
	} while (*delta_ns < measure_for_ns);

	uint64_t end_tsc = rdtsc_after();
	*delta_tsc = end_tsc - start_tsc;
}

static double measure_ns_per_tsc() {
	//return 0.36873315784891180779681008061743341386318206787109;
	
	/*
	double measure_for_s = 100;

	printf("measuring tsc rate for %lf s\n", measure_for_s);

	struct trusted_timer *timer;
	init_trusted_timer(&timer);

	uint64_t start_tsc = rdtsc_before();
	start_trusted_timer(timer);

	double elapsed_ns;
	do {
		elapsed_ns = stop_trusted_timer(timer);
	} while (elapsed_ns < measure_for_s * 1000 * 1000 * 1000);

	uint64_t end_tsc = rdtsc_after();

	free(timer);

	double ns_per_tsc = elapsed_ns / (double) (end_tsc - start_tsc);
	*/

	printf("measuring ns per tsc\n");

	struct trusted_timer *timer;
	init_trusted_timer(&timer);

	uint64_t x1, x2;
	double y1, y2;

	measure_ns_per_tsc_datapoint(timer, (double) 100 * 1000 * 1000 * 1000, &y1, &x1);
	measure_ns_per_tsc_datapoint(timer, (double) 200 * 1000 * 1000 * 1000, &y2, &x2);

	free(timer);

	double ns_per_tsc = (y2 - y1) / ((double) x2 - (double) x1);

	printf("estimating %.50lf ns per tsc\n", ns_per_tsc);
	
	double mhz = ((double) 1 / ns_per_tsc) * (double) 1000;
	printf("which is equivalent to %.50lf MHz\n", mhz);

	return ns_per_tsc;
}
	

void init_rdtsc_timer(struct rdtsc_timer *timer) {
    // assert CPU has the "invariant tsc" feature
    //
    // otherwise the timestamps per nanosecond will go up and down as the CPU
    // transitions between different states such as to save power by decreasing
    // clock speed, so this timer won't work.
    if (!CPU_FEATURE_PRESENT(INVARIANT_TSC)) {
        fprintf(stderr, "CPU does not support invariant TSC, aborting\n");
        exit(1);
    }

    //timer->cpu_hz = cpu_hz;
    //timer->ns_per_tsc = measure_ns_per_tsc();
    timer->ns_per_tsc = 0.36873315784891180779681008061743341386318206787109;
    timer->start = 0;

    //calibrate_overhead(timer);
}
