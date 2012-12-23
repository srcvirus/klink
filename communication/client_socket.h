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
	addrinfo server_info;

public:
	//ClientSocket(){;}
	ClientSocket()
	{
		server_host_name = "localhost";
		server_port_number = -1;
		server_info.ai_addr = NULL;
	}

	ClientSocket(const string& server, int port) :
			server_host_name(server), server_port_number(port)
	{
		server_info.ai_addr = NULL;
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

	void setServerInfo(addrinfo server_info)
	{
		this->server_info = server_info;
	}

	int send_data(char* buffer, int n_bytes, timeval* timeout = NULL);
	int receive_data(char** buffer);

	~ClientSocket();
};

int ClientSocket::connect_to_server()
{
	int ret_code = SUCCESS;


	if(server_info.ai_addr == NULL)
	{
		addrinfo hints, *res;
		memset(&hints, 0, sizeof(addrinfo));
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = 0;
		hints.ai_flags = 0;

		char str_server_port[10];
		sprintf(str_server_port, "%d", server_port_number);

		char str_host_name[300];
		strcpy(str_host_name, server_host_name.c_str());

		getaddrinfo(str_host_name, str_server_port, &hints, &res);
		server_info = (*res);
	}

	socket_fd = socket(server_info.ai_family, server_info.ai_socktype, server_info.ai_protocol);

	if (connect(socket_fd, server_info.ai_addr, server_info.ai_addrlen))
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
        if(ret_code < 0) ret_code = ERROR_DATA_SEND_FAILED;
	}
	else
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
                if(ret_code < 0) ret_code = ERROR_DATA_SEND_FAILED;
			}
			else
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
