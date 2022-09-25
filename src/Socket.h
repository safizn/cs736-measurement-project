#pragma once

#include <string>
#include <iostream>
#include "IPC.h"
#include "utility.cpp"

template<typename T> 
class Socket : public IPC<T> {
public: 
  Socket() = default;
  Socket(T data) {}
  // ~Socket() {}

  virtual int operator()(T data) override {
    utility::dummyLoop(1000000);
    return 0;
  }
};



