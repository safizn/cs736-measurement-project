#pragma once

#include <string>
#include <iostream>
#include <bits/stdc++.h> 

template<typename T = uint64_t>
class Buffer {
public:  

  // pass # of bytes to generate data: 4, 16, 64, 256, 1K, 4K, 16K, 64K, 256K, and 512K bytes
  static std::vector<T> generateDataInBytes(std::size_t bytes) {
    assert(bytes % sizeof(T) == 0);
    std::vector<T> data(bytes / sizeof(T));
    std::iota(data.begin(), data.end(), 0);
    std::shuffle(data.begin(), data.end(), std::mt19937{ std::random_device{}() });
    return data;
  }

  static size_t sizeof_vector(const typename std::vector<T>& vec) {
      return sizeof(T) * vec.size();
  }

};
