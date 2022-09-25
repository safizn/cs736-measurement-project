#pragma once

#include <string>
#include <iostream>
#include <bits/stdc++.h> 
#include <stdio.h>

using namespace std;
#define BYTE 8

const std::size_t kiB = 1024;
const std::size_t MiB = 1024 * kiB;
const std::size_t GiB = 1024 * MiB;

template<typename T = uint64_t>
class Buffer {
public:  

  Buffer(size_t numberOfBytes /* size in bytes */) : bytes(numberOfBytes) {
    assert(numberOfBytes % sizeof(T) == 0); // assume element block size is proportional to size requested
    this->data.resize(numberOfBytes / sizeof(T)); // set size of entire array
    generateRandomData(numberOfBytes);
  }

  // pass # of bytes to generate data: 4, 16, 64, 256, 1K, 4K, 16K, 64K, 256K, and 512K bytes
  void generateRandomData(std::size_t bytes) {
    // Specify the engine and distribution.
    mt19937 mersenne_engine {std::random_device{}()};  // Generates random integers
    uniform_int_distribution<T> dist {1, std::numeric_limits<T>::max()};
    auto gen = [&dist, &mersenne_engine](){
      return dist(mersenne_engine);
    };

    generate(data.begin(), data.end(), gen);
  }

  size_t sizeOfElement() {
    return sizeof(this->data[0])*BYTE;
  } 
  size_t sizeOfVector() {
    return Buffer<T>::sizeof_vector(this->data);
  }
  size_t numberOfElements() {
    return this->data.size();
  }

  void printSizeInfo() {
    assert(sizeof(T) == sizeof(this->data[0]));
    printf("Array details: \n\t Buffer entire size in bytes = %u \n\t # elements = %u \n\t # of bits per element = %u \n", sizeOfVector(), numberOfElements(), sizeOfElement());
  }

  void printBinaryData() {
    for(std::bitset<sizeof(T) * BYTE> i : this->data)  {
      cout << i << endl;
    }
    cout << endl;
  }

  static size_t sizeof_vector(const typename std::vector<T>& vec) {
      return sizeof(T) * vec.size();
  }

public:
  std::vector<T> data {0, 0};
  size_t bytes {0};
};
