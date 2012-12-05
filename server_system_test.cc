/*
 * system_test.cc
 *
 *  Created on: 2012-12-05
 *      Author: sr2chowd
 */

#include "communication/server_socket.h"
#include "communication/client_socket.h"
#include <stdlib.h>

int main(int argc, char* argv[])
{
	int port = atoi(argv[1]);

	fd_set connection_pool;
	fd_set read_connection_fds;
	int fd_max;

	FD_ZERO(&connection_pool);
	FD_ZERO(&read_connection_fds);
	ServerSocket* s_socket = new ServerSocket("localhost", 15000);

	s_socket->init_connection();
	fd_max = s_socket->getSocketFd();
	char* buffer;
	int buffer_length;

	while(true)
	{
		read_connection_fds = connection_pool;
		int n_select = select(fd_max + 1, &read_connection_fds, NULL, NULL, NULL);

		for(int i = 0; i <= fd_max; i++)
		{
			if(FD_ISSET(i, &read_connection_fds))
			{
				if( i == s_socket->getSocketFd() )
				{
					int connection_fd = s_socket->accept_connection();
					FD_SET(connection_fd, &connection_pool);
					if(connection_fd > fd_max)
						fd_max = connection_fd;
				}
			}
			else
			{
				buffer_length = s_socket->receive_data(i, &buffer);
				int messageType;
				//do the processing with the message
			}
		}
	}

	delete s_socket;
}



