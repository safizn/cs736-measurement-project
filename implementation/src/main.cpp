#include <iostream>
#include <cassert>
#include <chrono>
#include "utility.cpp"
#include "Pipe.h"
#include "Socket.h"
#include "SharedMemory.h"

using clockType = std::chrono::time_point<std::chrono::steady_clock>;

int main(int, char**) {
  assert(utility::checkCpp20Support() == 0); // check if compiler supports C++20 features

  std::cout << " Test " << std::endl;

  clockType start = std::chrono::steady_clock::now();
  utility::dummyLoop();
  clockType end = std::chrono::steady_clock::now();
  
  // Derive the duration
  std::chrono::duration<double, std::milli> fp_ms = end - start; 
  std::chrono::duration<unsigned long long, std::milli> int_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

  printf("Time in milliseconds: \n %d \n %lld", fp_ms, int_ms);

/*

  // initialize class instances
  Pipe i1{};
  Socket i2{};
  Shared_memory i3{};

  unsigned int data{1111};

  {  // profiler 1
     //----- start timer
     int status = i1.exchange(data); // will pipe data between processes and return.
     //----- end timer
  }  

  {  // profiler 2
     //----- start timer
     i2.exchange(data); // will send message using TCP between processes
     //----- end timer
  }
  
  {  // profiler 3
     //----- start timer
     i3.exchange(data); // will transfer data between processes using memory
     //----- end timer
  }
*/

  
}
