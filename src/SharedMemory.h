#pragma once

#include <string>
#include <iostream>
#include "IPC.h"
#include "utility.cpp"

template<typename T> 
class SharedMemory : public IPC<T> {
public: 
  SharedMemory() = default;
  SharedMemory(T data) {}
  // ~SharedMemory() {};

  virtual int exchange(T data) override{
    utility::dummyLoop(1000000);
    return 0;
  }
};


