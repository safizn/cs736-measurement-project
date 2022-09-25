#include <iostream>
#include <cassert>
#include <chrono>
#include <bitset> 

#include "Pipe.h"
#include "Socket.h"
#include "SharedMemory.h"
#include "Profiler.h"

using clockType = std::chrono::time_point<std::chrono::steady_clock>;

int main(int, char**) {
  assert(utility::checkCpp20Support() == 0); // check if compiler supports C++20 features

  // transfer data
  unsigned int data{0b111111111111111111111111111};  // in binary
  constexpr std::size_t data_size = std::numeric_limits<unsigned int>::digits;
  printf("Number of bytes in data = %u \n", sizeof(data));

  // instances
  Pipe<decltype(data)> i1{data, "pipe instance A"};
  Socket<decltype(data)> i2{data};
  SharedMemory<decltype(data)> i3{data};

  clockType start = std::chrono::steady_clock::now();
  i1.exchange(1);
  i2.exchange(10000000);
  i3.exchange(10000000);
  clockType end = std::chrono::steady_clock::now();
  
  // Derive the duration
  std::chrono::duration<double, std::milli> fp_ms = end - start; 
  std::chrono::duration<unsigned long long, std::milli> int_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

  printf("\nTime in milliseconds: %d or %lld \n", fp_ms, int_ms);
}
