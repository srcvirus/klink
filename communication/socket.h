/*
 * Socket.h
 *
 *  Created on: 2012-11-24
 *      Author: sr2chowd
 */

#ifndef SOCKET_H_
#define SOCKET_H_

#include <string>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <errno.h>
#include <unistd.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <memory.h>

using namespace std;

#define MAX_CONNECTIONS 1500
#define MAX_INCOMING_DATA_LENGTH 4096

class ABSSocket
{
protected:
	int socket_fd;

public:
	//virtual int send_data(int connection_fd, char* buffer, int n_bytes){return 0;}
	//virtual int receive_data(int connection_fd, char** buffer){return 0;}

	int getSocketFd(){ return socket_fd; }
	void close_socket(){ close(socket_fd); }
	void print_socket_info()
	{
		struct sockaddr_in sin;
		unsigned int addrLength = sizeof(sockaddr);
		getsockname(socket_fd, (sockaddr*)&sin, &addrLength);

		int port = ntohs(sin.sin_port);
		char hostName[100];
		gethostname(hostName, 100);

		printf("ADDRESS %s\n",hostName);
		printf("PORT %d\n", port);
	}
	virtual ~ABSSocket(){ close(socket_fd); };
};



#endif /* SOCKET_H_ */
