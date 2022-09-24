#pragma once

#include <string>
#include <iostream>


// interface of IPC mechanisms
class IPC {
  virtual int exchange() = 0;
};
