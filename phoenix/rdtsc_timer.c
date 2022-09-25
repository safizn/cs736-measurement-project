
#include "rdtsc_timer.h"
#include <stdlib.h>
#include <stdio.h>
#include <sys/platform/x86.h>
#include <sys/resource.h>


/**
 * Get the current CPU's current timestamp counter value using the rdtsc
 * instruction.
 *
 * adapted from:
 * https://github.com/fordsfords/rdtsc/blob/main/rdtsc.h
 */
uint64_t rdtsc() {
    uint32_t be, le;
    // explanation why volatile is necessary:
    // https://stackoverflow.com/a/26456845
    __asm__ volatile ("rdtsc" : "=a" (le), "=d" (be));
    return ((uint64_t) be) << 32 | ((uint64_t) le);
}


struct rdtsc_timer {
    uint64_t cpu_hz;
    /**
     * Start timestamp, or 0 if not in started state.
     */
    uint64_t start;
    /**
     * Estimated overhead of starting and stopping the timer in nanoseconds,
     * which is subtracted from measurements.
     */
    double overhead;
};

inline void start_rdtsc_timer(struct rdtsc_timer *timer) {
    timer->start = rdtsc();
}

/**
 * Part of `stop_timer` implementation that doesn't need to be inlined.
 */
double stop_rdtsc_timer_ool(struct rdtsc_timer *timer, uint64_t end);

inline double stop_rdtsc_timer(struct rdtsc_timer *timer) {
    uint64_t end = rdtsc();
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
    timer->start = 0;

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
        / (
            // convert from timestamps per second to timestamps per nanosecond,
            // producing a value within a few order of magnitudes of 1
            (double) timer->cpu_hz
            / (double) (1000 * 1000 * 1000)
        )
        // subtract the estimated overhead.
        - timer->overhead;
}

/**
 * Procedurally estimate and initialize an rdtsc timer's `overhead` value.
 */
static void calibrate_overhead(struct rdtsc_timer *timer) {
    int runs = 100000000;
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

void init_rdtsc_timer(struct rdtsc_timer *timer, uint64_t cpu_hz) {
    // assert CPU has the "invariant tsc" feature
    //
    // otherwise the timestamps per nanosecond will go up and down as the CPU
    // transitions between different states such as to save power by decreasing
    // clock speed, so this timer won't work.
    if (!CPU_FEATURE_PRESENT(INVARIANT_TSC)) {
        fprintf(stderr, "CPU does not support invariant TSC, aborting\n");
        exit(1);
    }

    timer->cpu_hz = cpu_hz;
    timer->start = 0;

    calibrate_overhead(timer);
}
