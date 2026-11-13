#pragma once

#include <iostream> // cout, cin
#include <sys/socket.h> // socket, bind, listen, accept
#include <netinet/in.h> // IPPROTO_TCP, sockaddr_in,
                        // htons/ntohs, INADDR_ANY
#include <unistd.h> // close
#include <arpa/inet.h> // inet_ntop/inet_atop
#include <string.h> // strlen
#include <semaphore.h> // sem_init
#include <pthread.h>

#define BUFFER_SIZE 1024

using namespace std;

class Environment_SocketServer {
public:
    Environment_SocketServer(int _port);
    ~Environment_SocketServer();
    void Accept();

private:
    int passive_socket;
    bool Communicate(int _socket);
    int port;

};

