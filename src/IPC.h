#pragma once

#include <string>
#include <iostream>

// interface of IPC mechanisms
template<typename T> 
class IPC {
public:
  // IPC() = default;
  // ~IPC();
  
  virtual int exchange(T) = 0;
};
