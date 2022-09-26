
#include "trusted_timer.h"
#include <stdlib.h>
#include <stdio.h>


int main() {
	int iterations = 100;
	
	struct trusted_timer *timer;
	init_trusted_timer(&timer);
	
	for (int i = 0; i < iterations; ++i) {
		start_trusted_timer(timer);
		double elapsed = stop_trusted_timer(timer);
	
		printf("elapsed = %lf ns\n", elapsed);
	}

	free(timer);
}

