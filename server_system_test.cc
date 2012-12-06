/*
 * system_test.cc
 *
 *  Created on: 2012-12-05
 *      Author: sr2chowd
 */

#include "communication/server_socket.h"
#include "communication/client_socket.h"
#include "communication/error_code.h"
#include "plnode/protocol/protocol.h"
#include "plnode/protocol/plexus/plexus_protocol.h"
#include "plnode/message/message.h"

#include <stdlib.h>

int main(int argc, char* argv[])
{
	int port = atoi(argv[1]);
	int error_code;

	Peer* this_peer = new Peer(port);
	ABSProtocol* plexus = new PlexusProtocol();
	//ABSProtocol* plexus = new PlexusProtocol(routing_table,index_table,cache,msgProcessor,this_peer);
	plexus->setContainerPeer(this_peer);

	fd_set connection_pool;
	fd_set read_connection_fds;
	int fd_max;

	FD_ZERO(&connection_pool);
	FD_ZERO(&read_connection_fds);

	ServerSocket* s_socket = this_peer->getServerSocket();
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
					if(buffer_length == 0)
					{
						s_socket->close_connection(i);
						FD_CLR(i, &connection_pool);
					}
					//do the processing with the message
					else
					{
						int messageType;
						ABSMessage* rcvd_message;

						memcpy(&messageType, buffer, sizeof(int));
						delete[] buffer;

						switch(messageType)
						{
						case MSG_PLEXUS_GET:
							rcvd_message = new MessageGET();
							rcvd_message->deserialize(buffer, buffer_length);
							break;

						case MSG_PLEXUS_PUT:
							rcvd_message = new MessagePUT();
							rcvd_message->deserialize(buffer, buffer_length);
							break;
						}

						printf("Received Message Type: %d, Overlay Hops = %d\n", rcvd_message->getMessageType(), rcvd_message->getOverlayHops());

						((PlexusProtocol*)plexus)->addToIncomingQueue(rcvd_message);

						ABSMessage* msg_to_process = ((PlexusProtocol*)plexus)->getIncomingQueueFront();
						plexus->getMessageProcessor()->processMessage(msg_to_process);

						plexus->setNextHop(msg_to_process);
						((PlexusProtocol*)plexus)->addToOutgoingQueue(msg_to_process);


						ABSMessage* msg_to_forward = ((PlexusProtocol*)plexus)->getOutgoingQueueFront();
						msg_to_forward->setOverlayHops(msg_to_forward->getOverlayHops() + 1);

						string dest_host = msg_to_forward->getDestHost();
						int dest_port = msg_to_forward->getDestPort();
						ClientSocket* c_socket = new ClientSocket(dest_host, dest_port);
						c_socket->connect_to_server();
						buffer = msg_to_forward->serialize(&buffer_length);
						c_socket->send_data(buffer, buffer_length);
					}

				}
			}
		}
	}

	delete s_socket;
}



