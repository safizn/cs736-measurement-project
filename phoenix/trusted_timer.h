/**
 * Interface for a more trusted, but less precise (than the rdtsc) timer to
 * validate the rdtsc timer against.
 *
 * Different implementations can be used at compile-time.
 */

#ifndef TRUSTED_TIMER
#define TRUSTED_TIMER


struct trusted_timer;

void init_trusted_timer(struct trusted_timer *timer);

/**
 * Start an initialized trusted timer.
 */
inline void start_trusted_timer(struct trusted_timer *timer);

/**
 * Stop a running trusted timer, and return the elapsed nanoseconds.
 */
inline double stop_trusted_timer(struct trusted_timer *timer);


#endif
