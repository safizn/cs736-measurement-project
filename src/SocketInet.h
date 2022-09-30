#pragma once

#include <bits/stdc++.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>

#include "utility.cpp"
#include "IPC.h"
#include "Buffer.h"

using namespace std;

// workfow:
// socket()
// bind()
// listen()
// select()
// accept()

template <size_t messageSize = 4 /*bytes*/, size_t chunkSize = 1 /*bytes*/, Mode mode = Mode::one_direction, typename T = uint64_t>
class SocketInet : public IPC<T> {
public:
  SocketInet() = default;
  SocketInet(Buffer<T> &data) : SocketInet(data) {}
  SocketInet(Buffer<T> &data, string ip_address = "127.0.0.1", int port = 5000, std::string label = "") : dataBuffer(data), ip_address(ip_address), port(port), label(label) {
    assert((messageSize % chunkSize) == 0);

    memset(&server_addr, 0, sizeof(server_addr));

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
    createClient();
    // int status = 0;
    // while ((wait(&status)) > 0)
    //   ;
  }
  }

  virtual int operator()() override {

  SINGLE_DIRECTION : // send message
  {
    unsigned char *p = this->dataBuffer.get_pointer();
    size_t remain{messageSize};
    int counter{0};

    while (remain > 0) {
      ssize_t written = send(this->sock, p, chunkSize, 0);
      if (written == -1) {
        printf("Client: Error when sending message to client - %s\n", strerror(errno));
        close(this->sock);
        close(client_fd);
        exit(1);
      }

      p += chunkSize;
      remain -= written;
      counter++;

      // { // testing
      //   std::bitset<BYTE> e = *(p);
      //   printf("[%u] written: %u bytes with # iterations: %u    Data: ", getpid(), written, counter);
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

      counter++;
      // { // testing
      //   std::bitset<BYTE> e = *((unsigned char *)tempBuffer);
      //   printf("[%u] received: %u bytes with # iterations: %u   Data: ", getpid(), received, counter);
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
    this->sock = socket(AF_INET, SOCK_STREAM, 0);

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htons(INADDR_ANY);
    server_addr.sin_port = htons(port);

    int address_length = sizeof(server_addr);

    if (this->sock <= 0) {
      printf("SERVER: Error when opening server socket.\n");
      exit(1);
    }

    rc = bind(this->sock, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (rc < 0) {
      printf("SERVER: Server bind error: %s\n", strerror(errno));
      close(this->sock);
      exit(1);
    }

    // accept connection
    rc = listen(this->sock, this->backlog);
    if (rc < 0) {
      printf("SERVER: Listen error: %s\n", strerror(errno));
      close(this->sock);
      exit(1);
    }

    printf("SERVER[%u]: Socket listening on port: %u \n", getpid(), port);
    client_fd = accept(this->sock, (struct sockaddr *)&this->sock, (socklen_t *)&address_length);
    if (client_fd < 0) {
      printf("SERVER: Accept error: %s\n", strerror(errno));
      close(this->sock);
      close(client_fd);
      exit(1);
    }

    // printf("SERVER: Connected to client at file descriptor: %u\n", client_fd);

  SINGLE_DIRECTION : // LISTEN TO CLIENT
  {
    ssize_t remain{messageSize};
    ssize_t received{0};
    int counter{0};
    do {
      // printf("SERVER: Wating for message...\n");
      // received = read(client_fd, tempBuffer, chunkSize);
      // int count = read(client_fd, tempBuffer, sizeof(chunkSize));
      received = recv(client_fd, tempBuffer, chunkSize, 0);
      if (received == -1) {
        printf("SERVER: Error when receiving message: %s\n", strerror(errno));
        close(this->sock);
        close(client_fd);
        exit(1);
      }

      remain -= received;

      // { // testing
      //   counter++;
      //   std::bitset<BYTE> e = *((unsigned char *)tempBuffer);
      //   printf("[%u] received: %u bytes with # iterations: %u    Data: ", getpid(), received, counter);
      //   cout << e << endl;
      // }
    } while (remain > 0);
  }

    if (mode == Mode::one_direction)
      return;
    else
      goto ROUND_TRIP;

  ROUND_TRIP : {
    // printf("SERVER: Respond to the client...\n");

    unsigned char *p = this->dataBuffer.get_pointer();
    size_t remain{messageSize};
    int counter{0};

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

      // { // testing
      //   std::bitset<BYTE> e = *(p);
      //   printf("[%u] written: %u bytes with # iterations: %u   Data: ", getpid(), written, counter);
      //   cout << e << endl;
      // }
    }
  }
  }

  void createClient() {
    // open client socket (same as server)
    this->sock = socket(AF_INET, SOCK_STREAM, 0);
    if (this->sock < 0) {
      printf("CLIENT: Socket error: %s\n", strerror(errno));
      exit(1);
    }

    /* get the address of the host */
    // struct hostent *hptr = gethostbyname("127.0.0.1"); /* localhost: 127.0.0.1 */
    // if (!hptr)
    //   printf("gethostbyname", 1);    /* is hptr NULL? */
    // if (hptr->h_addrtype != AF_INET) /* versus AF_LOCAL */
    //   printf("bad address family", 1);

    server_addr.sin_family = AF_INET;
    // server_addr.sin_addr.s_addr = ((struct in_addr *)hptr->h_addr_list[0])->s_addr;
    server_addr.sin_port = htons(port);

    rc = inet_pton(AF_INET, ip_address.c_str(), &server_addr.sin_addr);
    if (rc <= 0) {
      printf("CLIENT: Client inet_pton error. %s\n", strerror(errno));
      close(this->sock);
      exit(1);
    }

    // connect to server address
    printf("[Client] Connecting to server...\n");
    rc = connect(this->sock, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (rc < -1) {
      printf("CLIENT: Connect error. %s\n", strerror(errno));
      close(this->sock);
      exit(1);
    }
    if (rc == 0)
      printf("CLIENT[%u]: Connected to server.\n", getpid());
  }

public:
  std::string label{""};      // instance label
  pid_t pid_child{};          // process id
  Buffer<T> dataBuffer;       // random data to transfer
  char tempBuffer[chunkSize]; // receive buffer
  int sock{0};                // client/server sockets
  int rc{};                   // connection
  struct sockaddr_in server_addr {};
  // maximum number of client connections in queue
  const int backlog = 1;
  const int len{sizeof(sockaddr_un)}; // length of socket addresss.
  int client_fd{0};
  string ip_address{};
  int port{};
};
