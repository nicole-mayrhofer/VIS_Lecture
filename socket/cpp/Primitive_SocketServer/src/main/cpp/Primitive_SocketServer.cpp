#include "../headers/Primitive_SocketServer.h"

Environment_SocketServer::Environment_SocketServer(int _port) {          // constructor: passive socket
    cout << "Primitive_SocketServer::Primitive_SocketServer()" << endl;
    port = _port;

    passive_socket = socket(AF_INET, SOCK_STREAM, 0);     // erste socket
    if (passive_socket == -1) {
       perror("socket construction failed..");
       exit(EXIT_FAILURE);
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;      // adresse in apv4
    serverAddr.sin_addr.s_addr = INADDR_ANY;         // 0.0.0.0 is equivalent
    serverAddr.sin_port = htons(_port);       // irgendein freier port
    int res = ::bind(passive_socket, (struct sockaddr*) &serverAddr, sizeof(serverAddr));
    if (res == -1) {
       perror("bind failed..");
       exit(EXIT_FAILURE);
    }


    res =listen(passive_socket, 5);
    if (res == -1) {
       perror("listen failed..");
    }

}

Environment_SocketServer::~Environment_SocketServer() {
    cout << "Primitive_SocketServer::~Primitive_SocketServer()" << endl;
    close(passive_socket);


}


void Environment_SocketServer::Accept() {
    cout << "Primitive_SocketServer::Accept()" << endl;
    cout << "Server listening for clients ..." << endl;
    bool listening = true;
    while (listening) {
        int commSocket = accept(passive_socket, nullptr, nullptr);

        if (commSocket == -1) {
            perror("accept failed..");
        } else {
            listening = Communicate(commSocket);
        }
    }
}


bool Environment_SocketServer::Communicate(int _socket) {
    cout << "Primitive_SocketServer::Communicate()" << endl;
    sockaddr_in peerAddr;
    int peerAddrLen = sizeof(peerAddr);
    getpeername(_socket, (struct sockaddr*)&peerAddr, &peerAddrLen);

    int port = ntohs(peerAddr.sin_port);
    char ipPeer[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &peerAddr.sin_addr, ipPeer, sizeof(ipPeer));

    cout << "Connection established with peer.." << ipPeer << " on Port " << port << endl;

    int rVal = 0;
    bool listeningFlag = true;      // listening wird hier auf true gesetzt
    char buffer[BUFFER_SIZE];

    while (true) {
        rVal = recv(_socket, buffer, BUFFER_SIZE, 0);  //hier blockiert der Server
        if (rVal == -1) {
        perror("Recv failed...");
        break;
        } else if (rVal == 0) {
            perror("Connection closed...");
            break;
        }
        string msg = string(buffer, rVal); // wie viel Payload ist in diesem Buffer drinnen
        cout << "message received: " << msg << endl;

        // analyse msg
        if (msg == "drop") {
            break;
        }
        
        if (msg == "shutdown") {  //Server herunterfahren
            listeningFlag = false;
            break;
        }

        //send reply
        string reply = "Echo: " + msg;
        rVal = send(_socket, reply.c_str(), reply.size(), 0);
        if (rVal == -1) {
            perror("Send failed...");
            break;
        }
    }  // while true



    close(_socket);
    return listeningFlag;
}


int main(int _argc, char** _argv) {

    cout << "Starting Primitive_SocketServer ..."  << endl;

    if (_argc != 2) {           // first argument is running program, second is port
       cout << "Usage: ./Primitive_SocketServer <port>"  << endl;
       exit(EXIT_FAILURE);
    }

    int port =  atoi(_argv[1]); // w/o check before we don´t know if its the right env variable



    Environment_SocketServer server(port);

    server.Accept();

    cout << "Shutting down Primitive_SocketServer ..." << endl;
    return 0;
}

