#pragma once

#include <string>
#include <iostream>
#include "IPC.h"
#include "utility.cpp"

class Socket {
public: 
  Socket() {}
  ~Socket() {}

  int exchange(unsigned int data) {
    utility::dummyLoop(1000000);
    return 0;
  }
};



