#pragma once

#include <string>
#include <iostream>
// includes every standard library and STL include file
#include <bits/stdc++.h> 
// fork, pipe https://pubs.opengroup.org/onlinepubs/7908799/xsh/unistd.h.html
#include <unistd.h> 
#include <fcntl.h> // file control options - O_NONBLOCK
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

#include "Buffer.h"
#include "IPC.h"
#include "utility.cpp"

using namespace std;
// size_t buf_size = 1 << 18; // 2^18 = ~256KiB
const size_t buf_size = 4096;


template<typename T, size_t bufferSize = 1024*1024> 
class PipeDynamic : public IPC<T> {
public:
  PipeDynamic() = default;
  PipeDynamic(Buffer<T> & data) : PipeDynamic(data) {}
  PipeDynamic(Buffer<T> & data, std::string label = "") : sendBuffer(data), label(label) {
    // creates one-way communication channel (fd[1] write; fd[0] read)
    // TODO: consider flag 0 | O_NONBLOCK in pipe2()
    assert(pipe(this->fd) != -1); // 0 flag = same as pipe();
    
    // char* buf = (char*) malloc(buf_size); // reader malloc
    char buf[buf_size];

    pid_t pid = fork(); // inherits file descriptors 
    switch((this->pid_child = pid)) {
      case -1: perror("fork"); exit(EXIT_FAILURE); 

      case 0: { // child 
        close(this->fd[0]); // close unused read
        // ssize_t remaining = this->sendBuffer.bytes;

        for(unsigned long int block : this->sendBuffer.data) {
          const void* b = &block; 
          ssize_t written = write(this->fd[1], b, sizeof(block));
          cout << block << endl;
          if(written == -1) { cerr << "Error: " << strerror(errno) << endl; }
          // remaining -= written; 
        } 
        close(this->fd[1]); // reader will receive EOF
        exit(0);
      } 

      default:  // parent
        close(this->fd[1]);
        ssize_t read_size;
        read_size = read(this->fd[0], buf, buf_size);
        if(read_size == -1){ cerr << "Error: " << strerror(errno) << endl; }
        printf("Received: %u", buf);

        close(this->fd[0]);
    }
  }

  // exchange/send data
  virtual int operator()(T param = 0) override {
    return 0;
  }

public:
  std::string label {""}; // instance label
  int fd[2]; // file descriptor to pipe
  pid_t pid_child{}; // process id
  Buffer<T> sendBuffer; // data to transfer (for writing process)
};


