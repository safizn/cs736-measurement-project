#pragma once

#include <bits/stdc++.h>
#include <chrono>

using clockType = std::chrono::time_point<std::chrono::steady_clock>;

template <typename Functor>
class Profiler {
public:
  Profiler() = default;

  Profiler(std::string label = "") : label(label) {
    // instantiate
  }

  void benchmark(Functor instance) {
    this->start = std::chrono::steady_clock::now();
    {
      instance();
    }
    this->end = std::chrono::steady_clock::now();
  }

  void exportResult() {
    // Derive the duration
    // std::chrono::duration<double, std::milli> fp_ms = this->end - this->start;
    // std::chrono::duration<unsigned long long, std::milli> int_ms = std::chrono::duration_cast<std::chrono::milliseconds>(this->end - this->start);
    // printf("\nTime in milliseconds: %d or %lld \n", fp_ms, int_ms);
  }

public:
  std::string label{""};
  clockType end{};
  clockType start{};
};