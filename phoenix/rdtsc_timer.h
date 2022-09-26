/**
 * A precise timer we use for our experiments based on the rdtsc instruction.
 *
 * This breaks if the thread moves between CPUs, so the user must pin the
 * thread to a single CPU before using to ensure correctness.
 */


#ifndef RDTSC_TIMER
#define RDTSC_TIMER

#include <stdint.h>


/**
 * Precise timer for our experiments, based on the rdtsc instruction.
 */
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


/**
 * Initialize and calibrate an rdtsc timer.
 * 
 * The user should be able to find the cpu hz with lscpu.
 */
void init_rdtsc_timer(struct rdtsc_timer *timer, uint64_t cpu_hz);

/**
 * Start an initialized rdtsc timer.
 */
void start_rdtsc_timer(struct rdtsc_timer *timer);

/**
 * Stop a running rdtsc timer, and return the elapsed nanoseconds.
 */
double stop_rdtsc_timer(struct rdtsc_timer *timer);


#endif
