#pragma once

#include <string>
#include <iostream>

enum class Mode { one_direction,
                  round_trip };

// interface of IPC mechanisms
template <typename T>
class IPC {
public:
  // IPC() = default;
  // ~IPC();

  virtual int operator()() = 0;
  virtual int one_direction() = 0; // throughput test
  virtual int round_trip() = 0;    // latency test
};
