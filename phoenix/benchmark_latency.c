
#include "ipc_mechanism.h"
#include "tdtsc_timer.h"
#include <stdio.h>
#include <stdlib.h>


int main(int argc, char **argv) {
    int outer_repeat_count = 3;
    int calibrate_repeat_count = 3;
    int benchmark_repeat_count = 3;

    if (argc != 2) {
        fprintf(stderr, "expected args: [num_bytes]\n");
        exit(1);
    }
    int num_bytes = itoa(&argv[1]);

    printf("running latency benchmark with %i bytes\n", num_bytes);


    pin_cpu();


    struct rdtsc_timer timer;
    init_rdtsc_timer(&timer);

    struct ipc_latency_test *test;
    init_ipc_latency_test(&test, num_bytes);

    for (int outer_repeat = 0; outer_repeat < outer_repeat_count; ++outer_repeat) {
        printf("outer repeat %i/%i\n", outer_repeat + 1, outer_repeat_count);
        for (int calibrate_repeat = 0; calibrate_repeat < calibrate_repeat_count; ++calibrate_repeat) {
            printf("\tcalibrate repeat %i/%i\n", calibrate_repeat + 1, calibrate_repeat_count);
            
            calibrate_rdtsc_timer(&timer);
        }
        for (int benchmark_repeat = 0; benchmark_repeat < benchmark_repeat_count; ++benchmark_repeat) {
            printf("\tbenchmark repeat %i/%i\n", benchmark_repeat + 1, benchmark_repeat_count);

            start_rdtsc_timer(&timer);
            run_ipc_latency_test(test);
            double elapsed_ns = stop_rdtsc_timer(&timer);

            printf("\ttook %.50lf ns\n", elapsed_ns);

            double latency = elapsed_ns / 2;
            printf("\tlatency ns: %.50lf\n", latency);
        }
    }

    free_ipc_latency_test(test);
}
