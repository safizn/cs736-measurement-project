
#include "ipc_mechanism.h"
#include <unistd.h>


struct ipc_latency_test {};

void init_ipc_latency_test(struct ipc_latency_test **test, int num_bytes) {}

void run_ipc_latency_test(struct ipc_latency_test *test) {
    sleep(1);
}

void free_ipc_latency_test(struct ipc_latency_test *test) {}


struct ipc_throughput_test {};

int init_ipc_throughput_test(struct ipc_throughput_test **test) {
    return 2000000;
}

void run_ipc_throughput_test(struct ipc_throughput_test *test) {
    sleep(1);
}

void free_ipc_throughput_test(struct ipc_throughput_test *test) {}
