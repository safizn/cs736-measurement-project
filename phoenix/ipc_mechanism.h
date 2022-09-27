
#ifndef IPC_MECHANISM
#define IPC_MECHANISM


struct ipc_latency_test;

void init_ipc_latency_test(struct ipc_latency_test **test, int num_bytes);

void run_ipc_latency_test(struct ipc_latency_test *test);

void free_ipc_latency_test(struct ipc_latency_test *test);


struct ipc_throughput_test;

int init_ipc_throughput_test(struct ipc_throughput_test **test);

void run_ipc_throughput_test(struct ipc_throughput_test *test);

void free_ipc_throughput_test(struct ipc_throughput_test *test);



#endif
