#pragma once

#include <string>
#include <iostream>
// includes every standard library and STL include file
#include <bits/stdc++.h> 
// fork, pipe https://pubs.opengroup.org/onlinepubs/7908799/xsh/unistd.h.html
#include <unistd.h> 
#include <fcntl.h> // file control options - O_NONBLOCK

#include "IPC.h"
#include "utility.cpp"

using namespace std;

template<typename T = unsigned int, std::size_t data_size = std::numeric_limits<T>::digits> 
class Pipe {
public: 
  int fd[2]; // file descriptor to pipe
  pid_t pid_child; // process id
  T sendData{0}; // data to transfer (for writing process)
  T receiveData{0}; // data received (for reading process)

public:
  Pipe(T data) : sendData(data) {
    // creates one-way communication channel (fd[1] write; fd[0] read)
    assert(pipe2(this->fd, 0 | O_NONBLOCK) != -1); // 0 flag = same as pipe();
    
    this->pid_child = fork(); // inherits file descriptors 
    assert(this->pid_child != -1); // failed to fork (check errno)
    
    if(this->pid_child == 0) childAction(); 
    else parentAction();
  }

  ~Pipe() = default;

  void childAction(){ /* child writes */
    close(this->fd[0]); // close unused read
    assert(write(this->fd[1], &this->sendData, sizeof(T)) != -1);
    close(this->fd[1]); // reader will receive EOF
  }

  void parentAction() { /* parent reads */
    close(this->fd[1]); // close unused write
    assert(read(this->fd[0], &receiveData, sizeof(T)) != -1); // read once
    close(this->fd[0]);

    /** alternatively for handling string message:
    while (read(pipefd[0], &buf, 1) > 0)
      write(STDOUT_FILENO, &buf, 1);
    **/
  }

  int exchange(T data) {
    utility::dummyLoop(1000000);
    return 0;
  }


  T get_receiveData() { return this->receiveData; }

};



// Template specialization for string data
template <>
class Pipe <char*>
{
public:
	Pipe(){
    }
	char* add(char* a, char* b);
};

// char* Pipe<char*>::add(char* a, char* b)
// {
// 	return strcat(a,b);
// }


