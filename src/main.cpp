#include <iostream>
#include <cassert>
#include <bitset>
#include <functional>
#include <stdio.h>

#include "Buffer.h"
#include "PipeStatic.h"
#include "PipeDynamic.h"
#include "Socket.h"
#include "SocketInet.h"
#include "SharedMemory.h"
#include "Profiler.h"

int runTasks() {

  // // EXAMPLE USAGE PIPE 1: for throughput (one_direction())
  {
    constexpr size_t messageSize = Buffer<uint64_t>::_64K,
                     chunkSize = 1;
    // use randomized dataset
    Buffer<uint64_t> dataset{messageSize};
    // profiler instance
    Profiler<std::function<int()>> p{"profiler A label"};
    // mechanism instance & callback
    PipeDynamic<messageSize, chunkSize> instance{dataset, "pipe instance A"};
    auto func = [&instance]() -> int { return instance.one_direction(); };
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
    PipeDynamic<messageSize, chunkSize> instance{dataset, "pipe instance B"};
    auto func = [&instance]() -> int { return instance.round_trip(); };
    // run benchmark on callback & export results
    p.benchmark(func); // Note: benchmark logic should be in this function @ Profiuler.h
    p.exportResult();
  }

  // EXAMPLE USAGE SOCKET 1: for thoughput
  {
    constexpr size_t messageSize = Buffer<uint64_t>::_64K,
                     chunkSize = 1024 /** 16*/;
    constexpr Mode mode = Mode::one_direction; // Mode::one_direction or Mode::round_trip
    // use randomized dataset
    Buffer<uint64_t> dataset{messageSize};
    // profiler instance
    Profiler<std::function<int()>> p{"profiler A label"};
    // mechanism instance & callback
    Socket<messageSize, chunkSize, mode> instance{dataset, "socket instance A"};
    auto func = [&instance]() -> int {
      instance();
      instance.cleanup();
      return 0;
    };
    // run benchmark on callback & export results
    p.benchmark(func); // Note: benchmark logic should be in this function @ Profiuler.h
    p.exportResult();
  }
  // EXAMPLE USAGE SOCKET 1: for latency
  {
    constexpr size_t messageSize = Buffer<uint64_t>::_64K,
                     chunkSize = 1024 * 16;
    constexpr Mode mode = Mode::round_trip; // Mode::one_direction or Mode::round_trip
    // use randomized dataset
    Buffer<uint64_t> dataset{messageSize};
    // profiler instance
    Profiler<std::function<int()>> p{"profiler A label"};
    // mechanism instance & callback
    Socket<messageSize, chunkSize, mode> instance{dataset, "socket instance B"};
    auto func = [&instance]() -> int {
      instance();
      instance.cleanup();
      return 0;
    };
    // run benchmark on callback & export results
    p.benchmark(func); // Note: benchmark logic should be in this function @ Profiuler.h
    p.exportResult();
  }

  // EXAMPLE USAGE SOCKET INET 1: for throughput
  {
    constexpr size_t messageSize = Buffer<uint64_t>::_64K,
                     chunkSize = 1024 * 16;
    constexpr Mode mode = Mode::one_direction; // Mode::one_direction or Mode::round_trip
    // use randomized dataset
    Buffer<uint64_t> dataset{messageSize};
    // profiler instance
    Profiler<std::function<int()>> p{"profiler A label"};
    // mechanism instance & callback
    SocketInet<messageSize, chunkSize, mode> instance{dataset, "127.0.0.1", 3006, "instance x1"};
    auto func = [&instance]() -> int {
      instance();
      instance.cleanup();
      return 0;
    };
    // run benchmark on callback & export results
    p.benchmark(func); // Note: benchmark logic should be in this function @ Profiuler.h
    p.exportResult();
  }
  // EXAMPLE USAGE SOCKET INET 1: for latency
  {
    constexpr size_t messageSize = Buffer<uint64_t>::_64K,
                     chunkSize = 1024 * 16;
    constexpr Mode mode = Mode::round_trip; // Mode::one_direction or Mode::round_trip
    // use randomized dataset
    Buffer<uint64_t> dataset{messageSize};
    // profiler instance
    Profiler<std::function<int()>> p{"profiler X label"};
    // mechanism instance & callback
    SocketInet<messageSize, chunkSize, mode> instance{dataset, "127.0.0.1", 3007, "instance x2"};
    auto func = [&instance]() -> int {
      instance();
      instance.cleanup();
      return 0;
    };
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
