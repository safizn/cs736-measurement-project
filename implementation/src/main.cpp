#include <iostream>
#include <cassert>
#include "utility.cpp"
#include "Pipe.h"
#include "Socket.h"
#include "SharedMemory.h"

int main(int, char**) {
  assert(checkCpp20Support() == 0); // check if compiler supports C++20 features

  std::cout << " Test " << std::endl;


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
