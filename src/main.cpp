#include <iostream>
#include <cassert>
#include <bitset> 
#include <stdio.h>

#include "Buffer.h"
#include "PipeStatic.h"
#include "PipeDynamic.h"
#include "Socket.h"
#include "SharedMemory.h"
#include "Profiler.h"

int implementation1() {
  // transfer data
  unsigned long long int data{0b10101010101011111101010110000};  // in binary
  constexpr std::size_t data_size = std::numeric_limits<unsigned long long int>::digits;

  // instances
  PipeStatic<decltype(data)> i1{data, "pipe instance A"};
  Socket<decltype(data)> i2{data};
  SharedMemory<decltype(data)> i3{data};

  Profiler<decltype(i1)> p{"A"};
  p.run(i1);
  p.exportResult();
}

int implementation2() {
  constexpr size_t bufferLengthInBytes = 1024;
  Buffer<uint64_t> b1 {bufferLengthInBytes}; 
  // b1.printSizeInfo(); 
  // b1.printBinaryData();

  PipeDynamic<uint64_t> i{b1, "pipe instance B"};

  Profiler<decltype(i)> p{"B"};
  p.run(i);
  p.exportResult();
}

int main(int, char**) {
  assert(utility::checkCpp20Support() == 0); // check if compiler supports C++20 features

  // implementation1(); 

  implementation2();

  printf("Program ended gracefully.");
  return 0;
}
