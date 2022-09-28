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
  // // transfer data
  // unsigned long long int data{0b10101010101011111101010110000}; // in binary
  // constexpr std::size_t data_size = std::numeric_limits<unsigned long long int>::digits;

  // // instances
  // PipeStatic<decltype(data)> i1{data, "pipe instance A"};
  // Socket<decltype(data)> i2{data};
  // SharedMemory<decltype(data)> i3{data};

  // Profiler<decltype(i1)> p{"A"};
  // p.run(i1);
  // p.exportResult();
}

int implementation2() {

  constexpr size_t bufferLengthInBytes = Buffer<uint64_t>::_64K;
  Buffer<uint64_t> data{bufferLengthInBytes};
  // data.printSizeInfo();
  // data.printBinaryData();
  // data.testUsingPointer();

  { // PipeDynamic<4, 1> i{data, "pipe instance B"};
    PipeDynamic<bufferLengthInBytes, 1> i{data, "pipe instance B"};

    Profiler<decltype(i)> p{"B"};
    p.run(i);
    p.exportResult();
  }
  {
    PipeDynamic<bufferLengthInBytes, 16> i2{data, "pipe instance B2"};
    Profiler<decltype(i2)> p2{"B2"};
    p2.run(i2);
    p2.exportResult();
  }
  return 0;
}

int main(int, char **) {
  assert(utility::checkCpp20Support() == 0); // check if compiler supports C++20 features

  // implementation1();

  implementation2();

  printf("[Parent] program ended gracefully.\n");
  return 0;
}
