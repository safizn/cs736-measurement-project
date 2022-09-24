#include <iostream>
import Pipe;
import Socket;
import Shared_memory;

int main(int, char**) {
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

  
}
