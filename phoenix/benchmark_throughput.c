
#include "ipc_mechanism.h"
#include "rdtsc_timer.h"
#include "pin_cpu.h"
#include <stdio.h>
#include <stdlib.h>


int main(int argc, char **argv) {
	int outer_repeat_count = 3;
	int calibrate_repeat_count = 3;
	int benchmark_repeat_count = 3;

	printf("running throughput benchmark\n");


	pin_cpu();


	struct rdtsc_timer timer;
	init_rdtsc_timer(&timer);

	struct ipc_throughput_test *test;
	int num_bytes = init_ipc_throughput_test(&test);
	printf("num_bytes=%i\n", num_bytes);

	for (int outer_repeat = 0; outer_repeat < outer_repeat_count; ++outer_repeat) {
		printf("outer repeat %i/%i\n", outer_repeat + 1, outer_repeat_count);
		for (int calibrate_repeat = 0; calibrate_repeat < calibrate_repeat_count; ++calibrate_repeat) {
			printf("calibrate repeat %i/%i\n", calibrate_repeat + 1, calibrate_repeat_count);
			
			calibrate_rdtsc_timer(&timer);

			printf("\n");
		}
		for (int benchmark_repeat = 0; benchmark_repeat < benchmark_repeat_count; ++benchmark_repeat) {
			printf("benchmark repeat %i/%i\n", benchmark_repeat + 1, benchmark_repeat_count);

			start_rdtsc_timer(&timer);
			run_ipc_throughput_test(test);
			double elapsed_ns = stop_rdtsc_timer(&timer);

			printf("took %.50lf ns\n", elapsed_ns);

			double throughput_byps = (double) num_bytes * 1000 * 1000 * 1000 / elapsed_ns;
			printf("throughput Bps: %.50lf\n", throughput_byps);
		
			printf("\n");
		}
		printf("\n");
	}

	free_ipc_throughput_test(test);
}
