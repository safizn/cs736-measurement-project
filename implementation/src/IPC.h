
#ifndef IPC_H
#define IPC_H

#include <string>
#include <iostream>


// interface of IPC mechanisms
class IPC {
  virtual int exchange() = 0;
};

#endif // IPC_H
