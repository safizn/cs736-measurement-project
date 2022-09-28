#pragma once

#include <bits/stdc++.h>
#include <string>
#include <iostream>
#include <stdio.h>
#include <math.h> /* pow */

using namespace std;
#define BYTE 8

template <typename T = uint64_t>
class Buffer {
public:
  Buffer(size_t numberOfBytes /* size in bytes */) : bytes(numberOfBytes) {
    assert(numberOfBytes % sizeof(T) == 0);       // assume element block size is proportional to size requested
    this->data.resize(numberOfBytes / sizeof(T)); // set size of entire array
    generateRandomData(numberOfBytes);
  }

  // pass # of bytes to generate random data for.
  void generateRandomData(std::size_t bytes) {
    // Specify the engine and distribution.
    mt19937 mersenne_engine{std::random_device{}()}; // Generates random integers
    uniform_int_distribution<T> dist{1, std::numeric_limits<T>::max()};
    auto gen = [&dist, &mersenne_engine]() {
      return dist(mersenne_engine);
    };

    generate(data.begin(), data.end(), gen);
  }

  size_t sizeOfElement() { // in bits
    return sizeof(this->data[0]) * BYTE;
  }
  size_t sizeOfVector() {
    return Buffer<T>::sizeof_vector(this->data);
  }
  size_t numberOfElements() {
    return this->data.size();
  }

  unsigned char *get_pointer() {
    return (unsigned char *)this->data.data();
  }

  void printSizeInfo() {
    assert(sizeof(T) == sizeof(this->data[0]));
    printf("Array details: \n\t Buffer entire size in bytes = %u \n\t # elements = %u \n\t # of bits per element = %u \n", sizeOfVector(), numberOfElements(), sizeOfElement());
  }

  void printBinaryData() {
    for (std::bitset<sizeof(T) * BYTE> i : this->data) {
      cout << i << endl;
    }
    cout << endl;
  }

  static size_t sizeof_vector(const typename std::vector<T> &vec) {
    return sizeof(T) * vec.size();
  }

  void testUsingPointer() {
    // this->printBinaryData();
    uint64_t *p = (uint64_t *)this->get_pointer();
    // int e = 0;
    // while (e != this->numberOfElements()) {
    //   std::bitset<this->sizeOfElement()>;
    //   cout << p[e] << endl;
    //   e += sizeof(uint64_t);
    // }

    std::bitset<sizeof(T) *BYTE> o = this->data[0];
    std::bitset<sizeof(T) *BYTE> i = *(p);
    cout << o << endl;
    cout << i << endl;
  }

public:
  std::vector<T> data{0, 0};
  const size_t bytes{0};

  //  4, 16, 64, 256, 1K, 4K, 16K, 64K, 256K, and 512K bytes
  inline static constexpr std::size_t kiB = 1024;
  inline static constexpr std::size_t MiB = 1024 * kiB;
  inline static constexpr std::size_t GiB = 1024 * MiB;
  inline static constexpr std::size_t _4 = 4;
  inline static constexpr std::size_t _16 = 16;
  inline static constexpr std::size_t _64 = 64;
  inline static constexpr std::size_t _256 = 256;
  inline static constexpr std::size_t _1K = kiB * 1;
  inline static constexpr std::size_t _4K = kiB * 4;
  inline static constexpr std::size_t _16K = kiB * 16;
  inline static constexpr std::size_t _64K = kiB * 64;
  inline static constexpr std::size_t _256K = kiB * 256;
  inline static constexpr std::size_t _512K = kiB * 512;
};
