#include "../headers/Environment_SocketServer.h"

// constructor start
Environment_SocketServer::Environment_SocketServer(int _port) {

    // Dummy sensoren initialisieren
    mSensors.push_back(DummySensor("light", 1, 0, 1000));
    mSensors.push_back(DummySensor("noise", 1, 0, 120));
    mSensors.push_back(DummySensor("air",   3, 0, 500));
    // constructor: passive socket
    cout << "Environment_SocketServer::Environment_SocketServer()" << endl;
    port = _port;

    passive_socket = socket(AF_INET, SOCK_STREAM, 0); // erste socket
    if (passive_socket == -1) {
        perror("socket construction failed..");
        exit(EXIT_FAILURE);
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET; // adresse in apv4
    serverAddr.sin_addr.s_addr = INADDR_ANY; // 0.0.0.0 is equivalent
    serverAddr.sin_port = htons(_port); // irgendein freier port
    int res = ::bind(passive_socket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));
    if (res == -1) {
        perror("bind failed..");
        exit(EXIT_FAILURE);
    }


    res = listen(passive_socket, 5);
    if (res == -1) {
        perror("listen failed..");
    }
}

// constructor end




// deconstructor start
Environment_SocketServer::~Environment_SocketServer() {
    cout << "Environment_SocketServer::~Environment_SocketServer()" << endl;
    close(passive_socket);
}
// deconstructor end




void Environment_SocketServer::Accept() {
    cout << "Environment_SocketServer::Accept()" << endl;
    cout << "Server listening for clients ..." << endl;
    mListening = true;

    while (mListening) {
        socketParam* parameter = new socketParam();
        socklen_t addrLen;
    int commSocket = accept(passive_socket, &parameter->saddr, &addrLen);      // socket handle of client

    if (commSocket == -1) {
        perror("accept failed..");
        delete parameter;
        break;
    } else {
        parameter->socket = commSocket;
        parameter->server = this;

        // falls nicht in accept schon abgespeichert

        // socketParam* parameter = new socketParam();
        // parameter->socket = commSocket;
        // socklen_t addrLen;           // buffer size in
        // getpeername(commSocket, &parameter->saddr, &addrLen);        // success: 0, else: -1
        // listening = Communicate(commSocket);

        pthread_t threadID;
        int rVal = pthread_create(&threadID, nullptr, Environment_SocketServer::ClientCommunication, (void*)parameter);   // 0 is success
        if (rVal != NULL) {
            cerr << "pthread_create failed: " << strerror(rVal) << endl;
            close(commSocket);
            delete parameter;
            } else {
                 // thread joinen = waits for the thread to terminate
            pthread_detach(threadID);        // nullptr: hier könnte speicheraddr für exitcode stehen
            }

        }
    }
 }

/* the method that is called from thread needs to be static
 -> needs to get also the server as a parameter, because static fields cant access to object fields
 */

    void* Environment_SocketServer::ClientCommunication(void* _parameter) {
    socketParam* p = (socketParam*)_parameter;
    int clientSocket = p->socket;
    sockaddr clientAddr = p->saddr;
    Environment_SocketServer* server = p->server;
    delete p; // recommendation of AI - no garbage collector?
    if (!server->Communicate(clientSocket)) {
        server->mListening = false;
    }

    // thread beenden -> springen wieder in accept

    return nullptr;
}




bool Environment_SocketServer::Communicate(int _socket) {
    // comm aufrecht halten
    cout << "Environment_SocketServer::Commnicate()" << endl;
    sockaddr_in peerAddr;
    socklen_t peerAddrLen = sizeof(peerAddr);       //int peerAddrLen = sizeof(peerAddr);

    getpeername(_socket, (struct sockaddr *) &peerAddr, &peerAddrLen);

    int port = ntohs(peerAddr.sin_port);
    char ipPeer[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &peerAddr.sin_addr, ipPeer, sizeof(ipPeer));

    cout << "Connection established with peer.." << ipPeer << " on Port " << port << endl;

    int rVal = 0; // recieved Value
    bool listeningFlag = true;
    char buffer[BUFFER_SIZE];       // BUFFER_SIZE variable steht in h file

    while (true) {
        rVal = recv(_socket, buffer, BUFFER_SIZE, 0);
        if (rVal == -1) {
            perror("Recv failed ...");
            break;
        } else if (rVal == 0) {
            perror("Connection closed ...");
        }


        string message = string(buffer, rVal);      // rVal = wie viel payload ist im buffer - wenn wir den ges puffer lesen lesen wir auch alte daten
        cout << "Message received ..." << message << endl;

        // analyse message

        if (message == "drop") {        // verbindung zum client wird beendet
            break;
        }
        if (message == "shutdown") {     // server runterfahren - listeningflag auf false - passive socket schliessen
            listeningFlag = false;
            break;
        }

        if (message == "reqEnvDataTypes()#") {
            // send reply
            string reply;
            int sensorsLen = mSensors.size();
            for (DummySensor& s : mSensors) {
                reply += s.GetName();
                if (sensorsLen-- > 1) {
                    reply += ";";
                } else {
                    reply += "#";
                }
            }
            rVal = send(_socket, reply.c_str(), reply.length(), 0);     // TCP socket - was kann beim Schicken passieren? Kabelverbindung kaputt / Netzwerkverbindunh

        }

        if (message.rfind("reqEnvData(", 0) == 0 && message.substr(message.length() - 2) == ")#") {         // check if message starts with "reqEnvData(" and checks if it ends with ")#"
            string reply;
            int sensorsLen = mSensors.size();
            string data = message.substr(11, message.length() - 13); // message(first character of <datatype>, length of datatype
            for (DummySensor& s : mSensors) {
                if (s.GetName() == data) {
                    reply = std::to_string(time(nullptr)) + "|" + s.ReadValues() + "#";
                    break;
                }
                else if (&s == &mSensors.back()) {          // last element
                    reply += "no such datatype#";
                }
            }

            // send reply
            rVal = send(_socket, reply.c_str(), reply.length(), 0);     // TCP socket - was kann beim Schicken passieren? Kabelverbindung kaputt / Netzwerkverbindunh

        }

        if (message == "reqAll()#") {         // check if message starts with "reqEnvData(" and checks if it ends with ")#"

            int sensorsLen = mSensors.size();
            string reply = std::to_string(time(nullptr));
            for (DummySensor& s : mSensors) {
                reply += "|" + s.GetName() + ";" + s.ReadValues();
                }
            reply += '#';

            rVal = send(_socket, reply.c_str(), reply.length(), 0);     // TCP socket - was kann beim Schicken passieren? Kabelverbindung kaputt / Netzwerkverbindunh

        }


        if (rVal == -1) {                   // client beendet sich
            perror("Send failed ...");
            break;
        }

    }

    // communication ended, close the socket ...
    close(_socket);
    return listeningFlag;
}






int main(int _argc, char **_argv) {
    cout << "Starting Env_SocketServer ..." << endl;

    if (_argc != 2) {
        // first argument is running program, second is port
        cout << "Usage: ./Env_SocketServer <port>" << endl;
        exit(EXIT_FAILURE);
    }

    int port = atoi(_argv[1]); // w/o check before we don´t know if its the right env variable

    Environment_SocketServer server(port);      // constructor

    server.Accept();

    cout << "Shutting down Env_SocketServer ..." << endl;
    cin.get(); // server waits for answer before closed
    return 0;
}

/*
static void* ClientCommunication(void* _parameter) {
pthread_t threadID;
int rVal = pthread_create(&threadID, nullptr, Environment_SocketServer::ClientCommunication, nullptr);
}
*/