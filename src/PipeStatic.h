#pragma once

#include <string>
#include <iostream>
// includes every standard library and STL include file
#include <bits/stdc++.h> 
// fork, pipe https://pubs.opengroup.org/onlinepubs/7908799/xsh/unistd.h.html
#include <unistd.h> 
#include <fcntl.h> // file control options - O_NONBLOCK

#include "Buffer.h"
#include "IPC.h"
#include "utility.cpp"

using namespace std;

template<typename T = unsigned int, std::size_t data_size = std::numeric_limits<T>::digits> 
class PipeStatic : public IPC<T> {
public:
  PipeStatic() = default;
  PipeStatic(T data) : PipeStatic(data) {}
  PipeStatic(T data, std::string label = "") : sendData(data), label(label) {
    // creates one-way communication channel (fd[1] write; fd[0] read)
    // TODO: consider flag 0 | O_NONBLOCK
    assert(pipe2(this->fd, 0 ) != -1); // 0 flag = same as pipe();
    
    pid_t pid = fork(); // inherits file descriptors 
    assert(pid != -1); // failed to fork (check errno)
    
    if(pid == 0) { // child process
      printf("[%s] Parent id %d \n", label.c_str(), getpid());
      childAction(); // should block/wait for receiving data
      exit(0); 
    } else { // parent process
      this->pid_child = pid;
      printf("[%s] Child id %d \n", label.c_str(), getpid());
      parentAction(); 
      // continue back to profiler
    }
  }

  // ~PipeStatic();

  // exchange/send data
  virtual int operator()(T param = 0) override { // Parent begin writing data to child
    assert(write(this->fd[1], &this->sendData, sizeof(T)) != -1);
    close(this->fd[1]); // reader will receive EOF

    return 0;
  }


  // T get_receiveData() { return (this->receiveData); }

private: 
  void childAction(){ /* child wait to read */
    close(this->fd[1]); // close unused write
    // read till EOF
    if(read(this->fd[0], &this->receiveData, sizeof(T)) == -1){ cerr << "Error: " << strerror(errno) << endl; }
    printf("\n[%s] child read data = ", label.c_str()); 
    cout <<  std::bitset<32>(this->receiveData) << endl;
    close(this->fd[0]);
  }

  void parentAction() { /* parent writes */
    close(this->fd[0]); // close unused read
  }

public:
  std::string label {""}; // instance label
  int fd[2]; // file descriptor to pipe
  pid_t pid_child{}; // process id
  T sendData; // data to transfer (for writing process)
  T receiveData; // data received (for reading process)
};


