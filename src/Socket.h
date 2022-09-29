#pragma once

#include <bits/stdc++.h>
#include <string>
#include <iostream>
#include <string.h>
#include <errno.h>
#include <unistd.h> // for fork()
#include <stdio.h>  // for printf
#include <stdlib.h> // for exit()
#include <sys/socket.h>
#include <sys/un.h> // socket in Unix
#include <cstring>
#include <cstdlib>

#include "utility.cpp"
#include "IPC.h"
#include "Buffer.h"

using namespace std;

// socket files will be created in current working directory
#define SERVER_SOCK_PATH "unix_sock.server"
#define CLIENT_SOCK_PATH "unix_sock.client"

// workfow:
// socket()
// bind()
// listen()
// select()
// accept()

enum class Mode { one_direction,
                  round_trip };

template <size_t messageSize = 4 /*bytes*/, size_t chunkSize = 1 /*bytes*/, Mode mode = Mode::one_direction, typename T = uint64_t>
class Socket : public IPC<T> {
public:
  Socket() = default;
  Socket(Buffer<T> &data) : Socket(data) {}
  Socket(Buffer<T> &data, std::string label = "") : dataBuffer(data), label(label) {
    assert((messageSize % chunkSize) == 0);

    memset(&server_addr, 0, sizeof(server_addr));
    memset(&client_addr, 0, sizeof(client_addr));

    // bind to an address on FS for client to pick up
    // bind address to FS to share with client
    server_addr.sun_family = AF_UNIX;
    strcpy(server_addr.sun_path, SERVER_SOCK_PATH);
    client_addr.sun_family = AF_UNIX;
    strcpy(client_addr.sun_path, CLIENT_SOCK_PATH);
    // Bind client to FS address (Note: this binding could be skip if we want only send data to server without receiving)
    // unlink file before bind
    unlink(SERVER_SOCK_PATH);
    unlink(CLIENT_SOCK_PATH);
    // cleanup handler after exit
    std::atexit(cleanupStatic);

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
    createServer();
    cleanup();
    printf("[child process ended gracefully.]\n");
    exit(EXIT_SUCCESS);
  }

  PARENT : {
    sleep(1);
    createClient();
    // int status = 0;
    // while ((wait(&status)) > 0)
    //   ;
  }
  }

  virtual int operator()() override {
    // send message
    {
      unsigned char *p = this->dataBuffer.get_pointer();
      size_t remain{messageSize};
      while (remain > 0) {
        ssize_t written = send(this->sock, p, chunkSize, 0);
        if (written == -1) {
          printf("Client: Error when sending message to client.\n");
          close(this->sock);
          close(client_fd);
          exit(1);
        }

        p += chunkSize;
        remain -= written;

        // printf("[%u] written: %u with # iterations: %u\n", getpid(), written, counter);
        // { // testing
        //   std::bitset<BYTE> e = *(p);
        //   cout << e << endl;
        // }
      }
    }

    if (mode == Mode::one_direction)
      return 0;
    else
      goto ROUND_TRIP;

  ROUND_TRIP : { // listen to response

    printf("CLIENT: Wait for respond from server...\n");

    ssize_t received{0};
    int counter{0};
    do {
      received = recv(this->sock, tempBuffer, chunkSize, 0);
      if (received == -1) {
        printf("CLIENT: Recv Error. %s\n", strerror(errno));
        close(this->sock);
        exit(1);
      } else {
        // printf("CLIENT: Message received: %s\n", tempBuffer);
      }

      // counter++;
      // printf("[%u] received: %u with # iterations: %u\n", getpid(), received, counter);
      // { // testing
      //   std::bitset<BYTE> e = *((unsigned char *)tempBuffer);
      //   cout << e << endl;
      // }
    } while (received > 0);
  }

    return 0;
  }

  virtual int one_direction() override {
    return 0;
  }

  virtual int round_trip() override {
    return 0;
  }

  void cleanup() {
    close(this->sock);
    close(client_fd);
  }

private:
  void createServer() {
    // open socket stream (SOCK_STREAM type)
    this->sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (this->sock == -1) {
      printf("SERVER: Error when opening server socket.\n");
      exit(1);
    }

    rc = bind(this->sock, (struct sockaddr *)&server_addr, len);
    if (rc == -1) {
      printf("SERVER: Server bind error: %s\n", strerror(errno));
      close(this->sock);
      exit(1);
    }

    // accept connection
    rc = listen(this->sock, this->backlog);
    if (rc == -1) {
      printf("SERVER: Listen error: %s\n", strerror(errno));
      close(this->sock);
      exit(1);
    }

    printf("SERVER: Socket listening...\n");
    client_fd = accept(this->sock, (struct sockaddr *)&client_addr, (socklen_t *)&len);
    if (client_fd == -1) {
      printf("SERVER: Accept error: %s\n", strerror(errno));
      close(this->sock);
      close(client_fd);
      exit(1);
    }
    printf("SERVER: Connected to client at: %s\n", client_addr.sun_path);
    printf("SERVER: Wating for message...\n");

    // LISTEN TO CLIENT
    {
      ssize_t received{0};
      int counter{0};
      do {
        received = recv(client_fd, tempBuffer, chunkSize, 0);
        if (received == -1) {
          printf("SERVER: Error when receiving message: %s\n", strerror(errno));
          close(this->sock);
          close(client_fd);
          exit(1);
        } else {
          // printf("SERVER: Server received message: %s.\n", tempBuffer);
        }

        // counter++;
        // printf("[%u] received: %u with # iterations: %u\n", getpid(), received, counter);
        // { // testing
        //   std::bitset<BYTE> e = *((unsigned char *)tempBuffer);
        //   cout << e << endl;
        // }
      } while (received > 0);
    }

    if (mode == Mode::one_direction)
      return;
    else
      goto ROUND_TRIP;

  ROUND_TRIP : {
    // printf("SERVER: Respond to the client...\n");

    unsigned char *p = this->dataBuffer.get_pointer();
    size_t remain{messageSize};
    while (remain > 0) {
      ssize_t written = send(client_fd, p, chunkSize, 0);
      if (written == -1) {
        printf("SERVER: Error when sending message to client.\n");
        close(this->sock);
        close(client_fd);
        exit(1);
      }

      p += chunkSize;
      remain -= written;

      // printf("[%u] written: %u with # iterations: %u\n", getpid(), written, counter);
      // { // testing
      //   std::bitset<BYTE> e = *(p);
      //   cout << e << endl;
      // }
    }

    // printf("SERVER: Done!\n");
  }
  }

  void createClient() {
    // open client socket (same as server)
    this->sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (this->sock == -1) {
      printf("CLIENT: Socket error: %s\n", strerror(errno));
      exit(1);
    }

    rc = bind(this->sock, (struct sockaddr *)&client_addr, len);
    if (rc == -1) {
      printf("CLIENT: Client binding error. %s\n", strerror(errno));
      close(this->sock);
      exit(1);
    }

    // connect to server address
    rc = connect(this->sock, (struct sockaddr *)&server_addr, len);
    if (rc == -1) {
      printf("CLIENT: Connect error. %s\n", strerror(errno));
      close(this->sock);
      exit(1);
    }
    printf("CLIENT: Connected to server.\n");
  }

  static void cleanupStatic() {
    remove(SERVER_SOCK_PATH);
    remove(CLIENT_SOCK_PATH);
  }

public:
  std::string label{""};      // instance label
  pid_t pid_child{};          // process id
  Buffer<T> dataBuffer;       // random data to transfer
  char tempBuffer[chunkSize]; // receive buffer
  int sock{};                 // client/server sockets
  int rc{};                   // connection
  struct sockaddr_un server_addr {};
  struct sockaddr_un client_addr {};
  // maximum number of client connections in queue
  const int backlog = 10;
  const int len{sizeof(sockaddr_un)}; // length of socket addresss.
  int client_fd{};
};
