#pragma once

#include <iostream>
#include <string>
// includes every standard library and STL include file
#include <bits/stdc++.h>
// fork, pipe https://pubs.opengroup.org/onlinepubs/7908799/xsh/unistd.h.html
#include <err.h>
#include <fcntl.h> // file control options - O_NONBLOCK
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#include "Buffer.h"
#include "IPC.h"
#include "utility.cpp"

using namespace std;

#define READ_END 0
#define WRITE_END 1

template <size_t messageSize = 4 /*bytes*/, size_t chunkSize = 1 /*bytes*/, typename T = uint64_t>
class PipeDynamic : public IPC<T> {
public:
  PipeDynamic() = default;
  PipeDynamic(Buffer<T> &data) : PipeDynamic(data) {}
  PipeDynamic(Buffer<T> &data, std::string label = "") : dataBuffer(data), label(label) {
    assert((messageSize % chunkSize) == 0);
  }

  // exchange/send data
  virtual int operator()(T param = 0) override {
    int fd_ctp[2],
        fd_ptc[2];
    assert(pipe(fd_ctp) != -1);
    assert(pipe(fd_ptc) != -1);

    switch ((this->pid_child = fork())) {
    case -1:
      perror("fork");
      exit(EXIT_FAILURE);
    case 0:
      goto CHILD;
    default:
      goto PARENT;
    }

  CHILD : {
    close(fd_ctp[READ_END]);
    close(fd_ptc[WRITE_END]);
    this->write_end = fd_ctp[WRITE_END];
    this->read_end = fd_ptc[READ_END];

    unsigned char *p = this->dataBuffer.get_pointer();
    // { // testing
    //   std::bitset<BYTE> e = this->dataBuffer.data[1];
    //   std::bitset<BYTE> i = *(p + sizeof(uint64_t));
    //   cout << i << endl;
    //   cout << e << endl;
    // }
    size_t remain{messageSize};
    int counter{0};
    while (remain > 0) {
      ssize_t written = write(this->write_end, p, chunkSize);
      if (written == -1)
        cerr << "Error: " << strerror(errno) << endl;
      p += chunkSize;
      remain -= written;
      counter++;
      // printf("child written: %u with # iterations: %u\n", written, counter);
      // { // testing
      //   std::bitset<BYTE> e = *(p);
      //   cout << e << endl;
      // }
    }

    close(this->read_end);
    close(this->write_end);
    printf("child process ended gracefully\n");
    exit(EXIT_SUCCESS);
  }

  PARENT : {
    close(fd_ctp[WRITE_END]);
    close(fd_ptc[READ_END]);
    this->write_end = fd_ptc[WRITE_END];
    this->read_end = fd_ctp[READ_END];

    ssize_t received{0};
    int counter{0};
    do {
      received = read(this->read_end, tempBuffer, chunkSize);
      if (received == -1)
        cout << "Error[P]: "
             << strerror(errno) << endl;
      counter++;
      // printf("parent received: %u with # iterations: %u\n", received, counter);
      // { // testing
      //   std::bitset<BYTE> e = *((unsigned char *)tempBuffer);
      //   cout << e << endl;
      // }

    } while (received > 0);

    close(this->read_end);
    close(this->write_end);
  }

    return 0;
  }

public:
  std::string label{""}; // instance label
  pid_t pid_child{};     // process id
  Buffer<T> dataBuffer;  // data to transfer (for writing process)
  // 2 file descriptors to pipe (child to parent & parent to child direction)
  int read_end, write_end;
  unsigned char tempBuffer[chunkSize]; // receive buffer
};
