#include <iostream>
#include <cassert>
#include <bitset>
#include <functional>
#include <stdio.h>

#include "Buffer.h"
#include "PipeStatic.h"
#include "PipeDynamic.h"
#include "Socket.h"
#include "SharedMemory.h"
#include "Profiler.h"

int runTasks() {

  /*
  // EXAMPLE USAGE PIPE 1: for throughput (one_direction())
  {
    constexpr size_t messageSize = Buffer<uint64_t>::_64K,
                     chunkSize = 1;
    // use randomized dataset
    Buffer<uint64_t> dataset{messageSize};
    // profiler instance
    Profiler<std::function<int()>> p{"profiler A label"};
    // mechanism instance & callback
    PipeDynamic<messageSize, chunkSize> intance{dataset, "pipe instance"};
    auto func = [&intance]() -> int { return intance.one_direction(); };
    // run benchmark on callback & export results
    p.benchmark(func); // Note: benchmark logic should be in this function @ Profiuler.h
    p.exportResult();
  }
  // EXAMPLE USAGE PIPE 2: for latency (round_trip())
  {
    constexpr size_t messageSize = Buffer<uint64_t>::_64K,
                     chunkSize = 1024 * 16;
    // use randomized dataset
    Buffer<uint64_t> dataset{messageSize};
    // profiler instance
    Profiler<std::function<int()>> p{"profiler A label"};
    // mechanism instance & callback
    PipeDynamic<messageSize, chunkSize> intance{dataset, "pipe instance"};
    auto func = [&intance]() -> int { return intance.round_trip(); };
    // run benchmark on callback & export results
    p.benchmark(func); // Note: benchmark logic should be in this function @ Profiuler.h
    p.exportResult();
  }
  */

  // EXAMPLE USAGE SOCKET 1: for thoughput (one_direction())
  {
    constexpr size_t messageSize = Buffer<uint64_t>::_64K,
                     chunkSize = 1024 * 16;
    // use randomized dataset
    Buffer<uint64_t> dataset{messageSize};
    // profiler instance
    Profiler<std::function<int()>> p{"profiler A label"};
    // mechanism instance & callback
    Socket<messageSize, chunkSize> intance{dataset, "pipe instance"};
    auto func = [&intance]() -> int { return intance.one_direction(); };
    // run benchmark on callback & export results
    p.benchmark(func); // Note: benchmark logic should be in this function @ Profiuler.h
    p.exportResult();
  }

  return 0;
}

int main(int, char **) {
  assert(utility::checkCpp20Support() == 0); // check if compiler supports C++20 features

  runTasks();

  printf("[Main program ended gracefully.]\n");
  return 0;
}
