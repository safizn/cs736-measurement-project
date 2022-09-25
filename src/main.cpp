#include <iostream>
#include <cassert>
#include <chrono>
#include "Pipe.h"
#include "Socket.h"
#include "SharedMemory.h"
#include "Profiler.h"

using clockType = std::chrono::time_point<std::chrono::steady_clock>;

int main(int, char**) {
  assert(utility::checkCpp20Support() == 0); // check if compiler supports C++20 features

  // transfer data
  unsigned int data{101010101};  // TODO: get exact size

  // instances
  Pipe i1;
  Socket i2;
  SharedMemory i3;


  clockType start = std::chrono::steady_clock::now();
  i1.exchange(1);
  i2.exchange(10000000);
  i3.exchange(10000000);
  clockType end = std::chrono::steady_clock::now();
  
  // Derive the duration
  std::chrono::duration<double, std::milli> fp_ms = end - start; 
  std::chrono::duration<unsigned long long, std::milli> int_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

  printf("Time in milliseconds: %d or %lld", fp_ms, int_ms);

  
}
