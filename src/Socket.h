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

#include "utility.cpp"
#include "IPC.h"
#include "Buffer.h"

using namespace std;

#define SERVER_SOCK_PATH "unix_sock.server"
#define CLIENT_SOCK_PATH "unix_sock.client"

template <size_t messageSize = 4 /*bytes*/, size_t chunkSize = 1 /*bytes*/, typename T = uint64_t>
class Socket : public IPC<T> {
public:
  Socket() = default;
  Socket(Buffer<T> &data) : Socket(data) {}
  Socket(Buffer<T> &data, std::string label = "") : dataBuffer(data), label(label) {
    assert((messageSize % chunkSize) == 0);

    struct sockaddr_un server_addr;
    struct sockaddr_un client_addr;
    memset(&server_addr, 0, sizeof(struct sockaddr_un));
    memset(&client_addr, 0, sizeof(struct sockaddr_un));
    int rc; // connection

    // bind address to FS to share with client
    server_addr.sun_family = AF_UNIX;
    strcpy(server_addr.sun_path, SERVER_SOCK_PATH);
    client_addr.sun_family = AF_UNIX;
    strcpy(client_addr.sun_path, CLIENT_SOCK_PATH);

    switch ((this->pid_child = fork())) {
    case -1:
      perror("fork");
      exit(EXIT_FAILURE);
    case 0:
      sleep(1);
      goto CHILD;
    default:
      goto PARENT;
    }

  CHILD : {
    createServer(server_addr, client_addr, rc);
    printf("[child process ended gracefully.]\n");
    exit(EXIT_SUCCESS);
  }

  PARENT : {
    createClient(server_addr, client_addr, rc);
  }
  }

  virtual int operator()() override {
    utility::dummyLoop(1000000);
    return 0;
  }

  virtual int one_direction() override {
    return 0;
  }

  virtual int round_trip() override {
    return 0;
  }

private:
  void createServer(struct sockaddr_un &server_addr, struct sockaddr_un &client_addr, int &rc) {
    // maximum number of client connections in queue
    int backlog = 10;
    // open socket stream (SOCK_STREAM type)
    int server_sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_sock == -1) {
      printf("SERVER: Error when opening server socket.\n");
      exit(1);
    }
    // bind to an address on FS for client to pick up
    int len = sizeof(server_addr);
    // unline file before bind
    unlink(SERVER_SOCK_PATH);

    rc = bind(server_sock, (struct sockaddr *)&server_addr, len);
    if (rc == -1) {
      printf("SERVER: Server bind error: %s\n", strerror(errno));
      close(server_sock);
      exit(1);
    }

    // accept connection
    rc = listen(server_sock, backlog);
    if (rc == -1) {
      printf("SERVER: Listen error: %s\n", strerror(errno));
      close(server_sock);
      exit(1);
    }

    printf("SERVER: Socket listening...\n");
    int client_fd = accept(server_sock, (struct sockaddr *)&client_addr, (socklen_t *)&len);
    if (client_fd == -1) {
      printf("SERVER: Accept error: %s\n", strerror(errno));
      close(server_sock);
      close(client_fd);
      exit(1);
    }
    printf("SERVER: Connected to client at: %s\n", client_addr.sun_path);
    printf("SERVER: Wating for message...\n");

    // LISTEN TO CLIENT
    memset(tempBuffer, 0, 256);
    int byte_recv = recv(client_fd, tempBuffer, sizeof(tempBuffer), 0);
    if (byte_recv == -1) {
      printf("SERVER: Error when receiving message: %s\n", strerror(errno));
      close(server_sock);
      close(client_fd);
      exit(1);
    } else
      printf("SERVER: Server received message: %s.\n", tempBuffer);

    // RESPONED TO CLIENT
    printf("SERVER: Respond to the client...\n");
    memset(tempBuffer, 0, 256);
    strcpy(tempBuffer, "SERVER_MSG");
    rc = send(client_fd, tempBuffer, strlen(tempBuffer), 0);
    if (rc == -1) {
      printf("SERVER: Error when sending message to client.\n");
      close(server_sock);
      close(client_fd);
      exit(1);
    }
    printf("SERVER: Done!\n");

    close(server_sock);
    close(client_fd);
    remove(SERVER_SOCK_PATH);
  }

  void createClient(struct sockaddr_un &server_addr, struct sockaddr_un &client_addr, int &rc) {
    // open client socket (same as server)
    int client_sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (client_sock == -1) {
      printf("CLIENT: Socket error: %s\n", strerror(errno));
      exit(1);
    }

    // Bind client to FS address (Note: this binding could be skip if we want only send data to server without receiving)
    int len = sizeof(client_addr);

    unlink(CLIENT_SOCK_PATH);
    rc = bind(client_sock, (struct sockaddr *)&client_addr, len);
    if (rc == -1) {
      printf("CLIENT: Client binding error. %s\n", strerror(errno));
      close(client_sock);
      exit(1);
    }

    // connect to server address
    rc = connect(client_sock, (struct sockaddr *)&server_addr, len);
    if (rc == -1) {
      printf("CLIENT: Connect error. %s\n", strerror(errno));
      close(client_sock);
      exit(1);
    }
    printf("CLIENT: Connected to server.\n");

    // send message
    memset(tempBuffer, 0, sizeof(tempBuffer));
    strcpy(tempBuffer, "CLIENT_MSG");
    rc = send(client_sock, tempBuffer, sizeof(tempBuffer), 0);
    if (rc == -1) {
      printf("CLIENT: Send error. %s\n", strerror(errno));
      close(client_sock);
      exit(1);
    }
    printf("CLIENT: Sent a message to server.\n");

    // listen to response
    printf("CLIENT: Wait for respond from server...\n");
    memset(tempBuffer, 0, sizeof(tempBuffer));
    rc = recv(client_sock, tempBuffer, sizeof(tempBuffer), 0);
    if (rc == -1) {
      printf("CLIENT: Recv Error. %s\n", strerror(errno));
      close(client_sock);
      exit(1);
    } else
      printf("CLIENT: Message received: %s\n", tempBuffer);

    printf("CLIENT: Done!\n");

    close(client_sock);
    remove(CLIENT_SOCK_PATH);
  }

public:
  std::string label{""};      // instance label
  pid_t pid_child{};          // process id
  Buffer<T> dataBuffer;       // random data to transfer
  char tempBuffer[chunkSize]; // receive buffer
};
