#include <compare>

namespace utility {
  int  checkCpp20Support() {  return (10 <=> 20) > 0; }

  void dummyLoop() {
    for (size_t i = 0; i < 1000000; i++) { 
      i--; i++;
    }
  }
};
