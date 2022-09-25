#pragma once

#include <string>
#include <iostream>

// interface of IPC mechanisms
class IPC {
public:
  IPC() = default;
  virtual ~IPC();
  
  virtual int exchange(unsigned int) = 0;
};
