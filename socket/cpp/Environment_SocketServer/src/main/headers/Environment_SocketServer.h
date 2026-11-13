#include <atomic>
#include <string>
#include <vector>
#include <random>
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

class DummySensor {
public:
    DummySensor(const std::string& _name, int _valueCount, int _min, int _max)
        : mName(_name), mValueCount(_valueCount),
          mDist(_min, _max) {}            // range for the random values

    const std::string& GetName() const { return mName; }

    // Produces e.g. "123" (light) or "123;235;23" (air)
    std::string ReadValues() {
        std::string result;
        for (int i = 0; i < mValueCount; i++) {
            if (i > 0) result += ";";
            result += std::to_string(mDist(Generator()));
        }
        return result;
    }

private:
    std::string mName;
    int mValueCount;
    std::uniform_int_distribution<int> mDist;

    // One random generator shared by all sensors, seeded once
    static std::mt19937& Generator() {
        static std::mt19937 gen(std::random_device{}());
        return gen;
    }
};




using namespace std;

class Environment_SocketServer {
public:
    Environment_SocketServer(int _port);
    ~Environment_SocketServer();


    void Accept();

    static void* ClientCommunication(void* _parameter);

private:
    int passive_socket;
    bool Communicate(int _socket);
    int port;
    std::vector<DummySensor> mSensors;
    std::atomic<bool> mListening;

    struct socketParam {
        sockaddr saddr;
        int socket;                         // aktiver Socket zum Client
        Environment_SocketServer* server;
    };

};