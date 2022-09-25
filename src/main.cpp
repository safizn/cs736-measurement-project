#include <iostream>
#include <cassert>
#include <bitset> 

#include "Pipe.h"
#include "Socket.h"
#include "SharedMemory.h"
#include "Profiler.h"


int main(int, char**) {
  assert(utility::checkCpp20Support() == 0); // check if compiler supports C++20 features

  // transfer data
  unsigned long long int data{0b10101010101011111101010110000};  // in binary
  constexpr std::size_t data_size = std::numeric_limits<unsigned long long int>::digits;
  printf("Number of bytes in data = %u \n", sizeof(data));

  // instances
  Pipe<decltype(data)> i1{data, "pipe instance A"};
  Socket<decltype(data)> i2{data};
  SharedMemory<decltype(data)> i3{data};

  Profiler<decltype(i1)> p{"A"};
  p.run(i1);
  p.exportResult();


  return 0;
}
