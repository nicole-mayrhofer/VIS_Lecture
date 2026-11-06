#include <iostream> // cout, cin
#include <sys/socket.h> // socket, bind, listen, accept
#include <netinet/in.h> // IPPROTO_TCP, sockaddr_in,
// htons/ntohs, INADDR_ANY
#include <algorithm>
#include <unistd.h> // close
#include <arpa/inet.h> // inet_ntop/inet_atop
#include <string.h> // strerror
#include <semaphore.h> // sem_init
#include <pthread.h>

#define BUFFER_SIZE 1024

using namespace std;

int main(int _argc, char** _argv) {
	cout << "Starting Demo_SocketServer ..."  << endl;

	if (_argc != 2) {
		cout << "Usage: Demo_SocketServer <port>"  << endl;
		exit(EXIT_SUCCESS);
	}

	int port = atoi(_argv[1]);


	// SOCKET BAUEN
	int passiveSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (passiveSocket == -1) {
		perror ("socket construction failed");
		exit(EXIT_FAILURE);
	}

	// SOCKET AN DIE ADRESSE BINDEN
	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = INADDR_ANY;
	serverAddr.sin_port = htons(port);		//Port über den der Dienst erreichbar ist

	int res = bind(passiveSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	if (res == -1) {
		perror ("bind failed ...");
		exit(EXIT_FAILURE);
	}

	res = listen(passiveSocket, 5);
	if (res == -1) {
		perror ("listen failed ...");
	}

	cout << "Server listening for clients on port "<< port << "..." << endl;
	int commSocket = accept(passiveSocket, NULL, NULL);
	if (res == -1) {
		perror("acception failed ...");
	} else {
		// aus dem Socket die PeerAdress auslesen
		sockaddr_in peerAddr;
		int peerAddrLen = sizeof(peerAddr);
		getpeername(commSocket, (struct sockaddr*)&peerAddr, &peerAddrLen);

		int port = ntohs(peerAddr.sin_port);

		char ipPeer [INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &peerAddr.sin_addr, ipPeer, sizeof(ipPeer));

		cout << "Connection established with peer ..." << ipPeer << " on port " << port << endl;
	}

	cout << "Stopping Demo_SocketServer ..."  << endl;
    return 0;
}
