/*
 * system_test.cc
 *
 *  Created on: 2012-12-05
 *      Author: sr2chowd
 */

#include "communication/server_socket.h"
#include "communication/client_socket.h"
#include "communication/error_code.h"

#include <stdlib.h>

int main(int argc, char* argv[])
{
	int port = atoi(argv[1]);
	int error_code;

	fd_set connection_pool;
	fd_set read_connection_fds;
	int fd_max;

	FD_ZERO(&connection_pool);
	FD_ZERO(&read_connection_fds);
	ServerSocket* s_socket = new ServerSocket(port);
	error_code = s_socket->init_connection();
	FD_SET(s_socket->getSocketFd(), &connection_pool);

	if(error_code < 0)
	{
		print_error_message(error_code);
		exit(1);
	}

	fd_max = s_socket->getSocketFd();
	s_socket->print_socket_info();

	char* buffer;
	int buffer_length;

	while(true)
	{
		read_connection_fds = connection_pool;

		//puts("Waiting");
		int n_select = select(fd_max + 1, &read_connection_fds, NULL, NULL, NULL);
		//puts("Interrupt");
		if(n_select < 0)
		{
			puts("Select Error");
			exit(1);
		}

		for(int i = 0; i <= fd_max; i++)
		{
			if(FD_ISSET(i, &read_connection_fds))
			{
				if( i == s_socket->getSocketFd() )
				{
					int connection_fd = s_socket->accept_connection();
					//puts("Connection Established");

					if(connection_fd < 0)
					{
						print_error_message(connection_fd);
						exit(1);
					}
					FD_SET(connection_fd, &connection_pool);
					if(connection_fd > fd_max)
						fd_max = connection_fd;
				}
				else
				{
					buffer_length = s_socket->receive_data(i, &buffer);
					//printf("Received %d Bytes\n", buffer_length);
					if(buffer_length == 0) s_socket->close_connection(i);
					FD_CLR(i, &connection_pool);
					//do the processing with the message
				}
			}

		}
	}

	delete s_socket;
}



