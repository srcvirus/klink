/*
 * CS 454 Assignment 3
 * July 13, 2012
 * Group Member 1: Evgeny Shulyatyev (ID 20242378)
 * Group Member 2: Shihabur Rahman Chowdhury (ID 20453766)
 * */


#include <stdio.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <memory.h>
#include <netdb.h>
#include <ctype.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <string>

#include "comms.h"
#include "marshall.h"

using namespace std;

#define MAX_CONNECTIONS 10
#define MAX_HOSTNAME_LENGTH 500

#define SOCKET_CREATE_FAIL -1
#define SOCKET_BIND_FAIL -2
#define SOCKET_LISTEN_FAIL -3
#define MAX_CONNECTION_EXCEEDED -4
#define CONNECTION_ACCEPT_FAIL -5

#define RANDOM_PORT 0
#define PORT_NO RANDOM_PORT
//#define PORT_NO 48564

char hostName[MAX_HOSTNAME_LENGTH + 1];
char lengthBuffer[LENGTH_BUFFER_SIZE];


char *receiveMessage(int socketDescriptor, int *receiveCode)
{
	//function returns the number of bytes received or error code
	//IMPORTANT: REMEMBER TO DELETE ALLOCATED MEMORY LATER!
	int dataLength = 0;
	char *outData = NULL;

	//first, receive message length
	*receiveCode = recv(socketDescriptor, lengthBuffer, LENGTH_BUFFER_SIZE, 0);

	//make sure we received properly
	if(*receiveCode < 1 && *receiveCode != EWOULDBLOCK)
		return NULL;

	//now, get the length of the data:

	extractIntFromCharArray(0, lengthBuffer, &dataLength);

	//now dataLength contains the size of message data, allocate the buffer:
	outData = new char[dataLength + LENGTH_BUFFER_SIZE];

	//first, copy the length to the beginning of the array
	for(int i = 0; i<LENGTH_BUFFER_SIZE; i++)
		*(outData + i) = *(lengthBuffer +i);

	//now receive the data into the allocated buffer
	*receiveCode = recv(socketDescriptor, outData + LENGTH_BUFFER_SIZE, dataLength, 0);

	//make sure we received properly - NOTE WE MUST RECEIVE SOMETHING HERE!
	/*if(*receiveCode < 1)
	{
		*receiveCode = -1;
		return NULL;
	}*/
	return outData;
}


void printSocketInfo(int socketDescriptor)
{
	struct sockaddr_in sin;
	unsigned int addrLength = sizeof(sockaddr);
	getsockname(socketDescriptor, (sockaddr*)&sin, &addrLength);

	int port = ntohs(sin.sin_port);

	gethostname(hostName, MAX_HOSTNAME_LENGTH);

	printf("BINDER_ADDRESS %s\n",hostName);
	printf("BINDER_PORT %d\n", port);
}

char *getBinderAddress()
{
	return getenv(BINDER_ADDRESS);
}

char *getBinderPort()
{
	return getenv(BINDER_PORT);
}

int getPortNumber(int socketDescriptor)
{
	struct sockaddr_in sin;
	unsigned int addrLength = sizeof(sockaddr);
	getsockname(socketDescriptor, (sockaddr*)&sin, &addrLength);

	int port = ntohs(sin.sin_port);

	return port;

}


string getLocalHostName()
{
	gethostname(hostName, MAX_HOSTNAME_LENGTH);
	string output = hostName;
	return output;
}

void printErrorMessage(int errorCode)
{
	switch(errorCode)
	{
	case SOCKET_CREATE_FAIL:
		puts("Failed to Create Socket");
		break;

	case SOCKET_BIND_FAIL:
		puts("Failed to Bind to Socket");
		break;
	}
}

int createServerSocket()
{
	int socketDescriptor = -1;
	socketDescriptor = socket(AF_INET, SOCK_STREAM, 0);
	if(socketDescriptor < 0) return SOCKET_CREATE_FAIL;
	return socketDescriptor;
}

int createClientSocket(char *serverName, char *serverPort)
{
	struct addrinfo hints, *res;
	int socketDescriptor = -1;

	memset(&hints, 0, sizeof(addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	getaddrinfo(serverName, serverPort, &hints, &res);

	socketDescriptor = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	int connectResult = connect(socketDescriptor, res->ai_addr, res->ai_addrlen);
	if(connectResult < 0)
		socketDescriptor = connectResult;

	return socketDescriptor;
}

int bindToSocket(int socketDescriptor, int portNumber)
{
	sockaddr_in localSocketAddress;
	localSocketAddress.sin_family = AF_INET;
	localSocketAddress.sin_addr.s_addr = INADDR_ANY;
	localSocketAddress.sin_port = htons(portNumber);
	memset(localSocketAddress.sin_zero, 0, sizeof(localSocketAddress.sin_zero));

	if(bind(socketDescriptor, (sockaddr*)&localSocketAddress, sizeof(sockaddr)) < 0)
	{
		close(socketDescriptor);
		return SOCKET_BIND_FAIL;
	}
	return socketDescriptor;
}

int listenToSocket(int socketDescriptor, int connections)
{
	if(connections > MAX_CONNECTIONS)
		return MAX_CONNECTION_EXCEEDED;

	if(listen(socketDescriptor, connections) < 0)
	{
		close(socketDescriptor);
		return SOCKET_LISTEN_FAIL;
	}
	return socketDescriptor;
}

int connectServerSocket(int portNumber)
{
	int socketDescriptor = createServerSocket();

	if(socketDescriptor < 0)
		return socketDescriptor;

	socketDescriptor = bindToSocket(socketDescriptor, portNumber);

	if(socketDescriptor < 0)
		return socketDescriptor;

	socketDescriptor = listenToSocket(socketDescriptor, MAX_CONNECTIONS);

	if(socketDescriptor < 0)
		return socketDescriptor;

	return socketDescriptor;
}

int acceptConnection(int socketDescriptor)
{
	int connectionDescriptor = accept(socketDescriptor, NULL, NULL);
	if(connectionDescriptor < 0)
		return CONNECTION_ACCEPT_FAIL;

	return connectionDescriptor;
}



