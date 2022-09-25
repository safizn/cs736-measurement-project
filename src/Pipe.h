#pragma once

#include <string>
#include <iostream>
#include "IPC.h"
#include "utility.cpp"

class Pipe {
public:
  Pipe() = default;
  ~Pipe() = default;

  int exchange(unsigned int data) {
    utility::dummyLoop(1000000);
    return 0;
  }
};

