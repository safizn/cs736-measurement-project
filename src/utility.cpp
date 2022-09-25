#pragma once 

#include <compare>

namespace utility {
  int  checkCpp20Support() {  return (10 <=> 20) > 0; }

  void dummyLoop(size_t length) {
    for (size_t i = 0; i < length; i++) { 
      i--; i++;
    }
  }


};
