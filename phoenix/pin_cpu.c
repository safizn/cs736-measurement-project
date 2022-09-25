
#define _GNU_SOURCE

#include "pin_cpu.h"
#include <stdio.h>
#include <pthread.h>
#include <sched.h>
#include <errno.h>

void pin_cpu(void) {
    printf("pinning thread to CPU\n");

    // get current thread and CPU
    pthread_t my_thread = pthread_self();
    int my_cpu = sched_getcpu();

    cpu_set_t cpu_set;
    CPU_ZERO(&cpu_set);
    CPU_SET(my_cpu, &cpu_set);

    // pin current thread to current cpu
    int e = pthread_setaffinity_np(my_thread, sizeof(cpu_set_t), &cpu_set);
    if (e) {
        fprintf(stderr, "error %i from pthread_setaffinity_np, aborting\n", e);
        exit(1);
    }
}
