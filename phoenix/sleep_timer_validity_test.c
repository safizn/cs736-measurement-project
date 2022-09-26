
#include "rdtsc_timer.h"
#include "trusted_timer.h"
#include "pin_cpu.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>


int main() {
    int cpu_hz = 2700 * 1000 * 1000;
    int sleep_for = 1;
    int test_runs = 10;

    pin_cpu();

    struct rdtsc_timer rdtsc_timer;
    init_rdtsc_timer(&rtdsc_timer, CPU_HZ);

    struct trusted_timer trusted_timer;
    init_trusted_timer(&trusted_timer);

    for (int i = 0; i < test_runs; ++i) {
        printf("running test %i/%i\n", i + 1, test_runs);

        start_trusted_timer(&trusted_timer);
        start_rdtsc_timer(&rdtsc_timer);

        sleep(sleep_for);

        double elapsed_rdtsc = stop_rdtsc_timer(&rdtsc_timer);
        double elapsed_trusted = stop_trusted_timer(&trusted_timer);

        printf(
            "- elapsed_rdtsc=%f\n- elapsed_trusted=%f\n- delta=%f\n"
            elapsed_rdtsc,
            elapsed_trusted,
            abs(elapsed_rdtsc - elapsed_trusted),
        );
    }
}
