/*
 * client_socket.h
 *
 *  Created on: 2012-11-24
 *      Author: sr2chowd
 */

#ifndef CLIENT_SOCKET_H_
#define CLIENT_SOCKET_H_

#include "socket.h"
#include "error_code.h"
#include <fcntl.h>
#include <error.h>

class ClientSocket: public ABSSocket
{
	string server_host_name;
	int server_port_number;

public:
	//ClientSocket(){;}
	ClientSocket()
	{
		server_host_name = "localhost";
		server_port_number = -1;
	}

	ClientSocket(const string& server, int port) :
			server_host_name(server), server_port_number(port)
	{
		;
	}

	int connect_to_server();

	void setServerHostName(const string& hostname)
	{
		server_host_name = hostname;
	}
	void setServerPortNumber(int port)
	{
		server_port_number = port;
	}
	string getServerHostName()
	{
		return server_host_name;
	}
	int getServerPortNumber()
	{
		return server_port_number;
	}

	int send_data(char* buffer, int n_bytes, timeval* timeout = NULL);
	int receive_data(char** buffer);

	~ClientSocket();
};

int ClientSocket::connect_to_server()
{
	int ret_code = SUCCESS;
	struct addrinfo hints, *res;

	memset(&hints, 0, sizeof(addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	char str_server_port[10];
	sprintf(str_server_port, "%d", server_port_number);

	getaddrinfo(server_host_name.c_str(), str_server_port, &hints, &res);

	socket_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	fcntl(socket_fd, F_SETFL, O_NONBLOCK);

	if (connect(socket_fd, res->ai_addr, res->ai_addrlen))
	{
		if (errno != EINPROGRESS)
			ret_code = errno;
		if (ret_code < 0)
			ret_code = ERROR_SERVER_CONNECTION_FAIL;
	}

	return ret_code;
}

int ClientSocket::receive_data(char** buffer)
{
	*buffer = new char[MAX_INCOMING_DATA_LENGTH];
	int n_bytes = recv(socket_fd, *buffer, MAX_INCOMING_DATA_LENGTH, 0);
	return n_bytes;
}

int ClientSocket::send_data(char* buffer, int n_bytes, timeval* timeout)
{
	int ret_code;
	if (timeout == NULL)
	{
		ret_code = send(socket_fd, buffer, n_bytes, 0);
	} else
	{
		fd_set write_connection;
		FD_ZERO(&write_connection);
		FD_SET(socket_fd, &write_connection);
		int fd_max = socket_fd;

		if (select(fd_max + 1, NULL, &write_connection, NULL, timeout) != -1)
		{
			if (FD_ISSET(socket_fd, &write_connection))
			{
				ret_code = send(socket_fd, buffer, n_bytes, 0);
			} else
			{
				ret_code = ERROR_CONNECTION_TIMEOUT;
			}
		}
	}
	return ret_code;
}

ClientSocket::~ClientSocket()
{
	;
}

#endif /* CLIENT_SOCKET_H_ */
