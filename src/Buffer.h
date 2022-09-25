#pragma once

#include <string>
#include <iostream>
#include <bits/stdc++.h> 
#include <stdio.h>

using namespace std;

template<typename T = uint64_t>
class Buffer {
public:  

  Buffer(size_t size /* size in bytes */) {
    this->data = Buffer<T>::generateDataInBytes(size);
  }

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

  void printSizeInfo() {
    printf("Buffer size in Bytes: %i \n", Buffer<T>::sizeof_vector(this->data));
    printf("\nBuffer single element size in bytes %i\n", sizeof(this->data[0])); // returns size in bytes of each element
  }

  void printBinaryData() {
    for(std::bitset<sizeof(T) * 8> i : this->data) 
      cout << i << endl;
  }

public:
  std::vector<T> data {};
};
