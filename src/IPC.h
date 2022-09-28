#pragma once

#include <string>
#include <iostream>

// interface of IPC mechanisms
template <typename T>
class IPC {
public:
  // IPC() = default;
  // ~IPC();

  virtual int operator()() = 0;
  virtual int one_direction() = 0;
  virtual int round_trip() = 0;
};
