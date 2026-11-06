#include <iostream> // cout, cin
#include <sys/socket.h> // socket, bind, listen, accept
#include <netinet/in.h> // IPPROTO_TCP, sockaddr_in,
                        // htons/ntohs, INADDR_ANY
#include <unistd.h> // close
#include <arpa/inet.h> // inet_ntop/inet_atop
#include <string.h> // strerror
#include <semaphore.h> // sem_init
#include <pthread.h>

#define BUFFER_SIZE 1024

using namespace std;

int main(int _argc, char** _argv) {
	cout << "Starting Demo_SocketClient ..."  << endl;

	if (_argc != 3) {
		cout << "Usage: Demo_SocketClient <IP address> <port>" << endl;
		exit(EXIT_FAILURE);
	}

	int commSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (commSocket == -1) {
		perror("socket creation failed ...");
		exit(EXIT_FAILURE);
	}


	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(atoi(_argv[2]));		//Port über den der Dienst erreichbar ist
	int res = inet_pton(AF_INET, _argv[1], &serverAddr.sin_addr);
	if (res == -1) {
		perror("inet_pton failed; IP could not be converted");
		exit(EXIT_FAILURE);
	}

	res = connect(commSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	if (res == -1) {
		perror("connect failed ...");
	} else {
		cout << "Connection established ..." << endl;
	}

	close(commSocket);

	cout << "Stopping Demo_SocketClient ..."  << endl;

	return 0;
}
