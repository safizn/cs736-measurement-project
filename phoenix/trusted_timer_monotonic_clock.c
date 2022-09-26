/**
 * Implementation of the `trusted_timer.h` header with the `time.h`
 * `CLOCK_MONOTONIC`.
 */

#include "trusted_timer.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>


struct trusted_timer {
    int running;
    int error;
    struct timespec start;
};

void init_trusted_timer(struct trusted_timer *timer) {
    timer->running = 0;
}

void start_trusted_timer(struct trusted_timer *timer) {
    timer->running = 1;
    timer->error = clock_gettime(CLOCK_MONOTONIC, &timer->start);
}

static double stop_trusted_timer_ool(struct trusted_timer *timer, int end_error, struct timespec end);

double stop_trusted_timer(struct trusted_timer *timer) {
    struct timespec end;
    int end_error = clock_gettime(CLOCK_MONOTONIC, &end);


}

static double stop_trusted_timer_ool(struct trusted_timer *timer, int end_error, struct timespec end) {
    // assert actually is running, and the `clock_gettime` calls didn't error
    if (!timer->running) {
        fprintf(stderr, "stop_trusted_timer on non-running timer, aborting\n");
        exit(1);
    }
    if (timer->error) {
        fprintf(stderr, "clock_gettime gave error %i on starting, aborting\n", timer->error);
        exit(1);
    }
    if (end_error) {
        fprintf(stderr, "clock_gettime gave error %i on ending, aborting\n", end_error);
        exit(1);
    }

    // return timer to non-running state
    timer->running = 0;

    return (double) (
        (end.tv_sec - timer->start.tv_sec)
        * 1000 * 1000 * 1000
        + (end.tv_nsec - timer->start.tv_nsec)
    );
}
