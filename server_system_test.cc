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
#include "plnode/message/control/peer_init_message.h"
#include "plnode/protocol/plexus/plexus_message_processor.h"

#include <stdlib.h>
#include <pthread.h>

//Globals
Peer* this_peer;
ABSProtocol* plexus;

int fd_max;
fd_set connection_pool;
fd_set read_connection_fds;

ServerSocket* s_socket = NULL;


void system_init();
void cleanup();

void *listener_thread(void*);
void *forwarding_thread(void*);
void *processing_thread(void*);

/*int read_port(const char* hosts_file)
{
	FILE* hosts_ptr = fopen(hosts_file, "r");
	char host_name[200];
	int port;
	int n_hosts;
	fscanf(hosts_ptr, "%d", &n_hosts);

	for(int i = 0; i < n_hosts; i++)
	{
		fscanf(hosts_ptr, "%s %d", host_name, &port);
		if(strncmp(this_peer->getHostName().c_str(), host_name, strlen(this_peer->getHostName().c_str())))
			return port;
	}

	return ERROR_NOT_IN_HOSTS_FILE;
}*/

/*void process_and_forward(ABSMessage* rcvd_message) {
    printf("Processing Message, Type: %d, Overlay Hops = %d\n", rcvd_message->getMessageType(), rcvd_message->getOverlayHops());
    //plexus->printRoutingTable();
    bool forward = plexus->setNextHop(rcvd_message);
    if (!forward) {
        printf("Local Processing...\n");
        ((PlexusProtocol*) plexus)->processMessage(rcvd_message);
        return;
    } else {
        printf("Forwarding To %s:%d\n", rcvd_message->getDestHost().c_str(), rcvd_message->getDestPort());

        string dest_host = rcvd_message->getDestHost();
        int dest_port = rcvd_message->getDestPort();
        ClientSocket* c_socket = new ClientSocket(dest_host, dest_port);

        c_socket->connect_to_server();
        int buffer_length = 0;
        char* buffer = rcvd_message->serialize(&buffer_length);
        c_socket->send_data(buffer, buffer_length);
        c_socket->close_socket();
        delete c_socket;
    }
}*/

int main(int argc, char* argv[])
{
    system_init();
    pthread_t listener, processor, forwarder;

    pthread_create(&listener, NULL, listener_thread, NULL);
    pthread_create(&forwarder, NULL, forwarding_thread, NULL);
    pthread_create(&processor, NULL, processing_thread, NULL);

    pthread_join(listener, NULL);
    pthread_join(forwarder, NULL);
    pthread_join(processor, NULL);
    
    cleanup();
}

void system_init()
{
	int error_code;
	puts("Initializing the System");

	/* creating the peer container */
	this_peer = new Peer(GlobalData::host_file_name.c_str());
	if(this_peer->getListenPortNumber() == -1)
	{
		puts("Port Number Not Found");
		exit(1);
	}
	/* creating the plexus protocol object */
	plexus = new PlexusProtocol();

	/* creating the message processor for plexus */
	PlexusMessageProcessor* msg_processor = new PlexusMessageProcessor();

	/* setting the message processor's protocol */
	msg_processor->setContainerProtocol(plexus);

	/* setting the container and message processor for plexus */
	plexus->setMessageProcess(msg_processor);
	plexus->setContainerPeer(this_peer);

	/* setting the protocol of the peer */
	this_peer->setProtocol(plexus);

	/* initializing the connection sets */
	FD_ZERO(&connection_pool);
	FD_ZERO(&read_connection_fds);

	/* creating the server socket for accepting connection from other peers */
	s_socket = this_peer->getServerSocket();
	error_code = s_socket->init_connection();

	FD_SET(s_socket->getSocketFd(), &connection_pool);

	if (error_code < 0)
	{
		print_error_message(error_code);
		exit(1);
	}

	fd_max = s_socket->getSocketFd();
	s_socket->print_socket_info();
}

void cleanup()
{
	delete this_peer;
	this_peer = NULL;
	plexus = NULL;
}

void *listener_thread(void* args)
{
	puts("Starting Listener Thread");

	int buffer_length;
	char* buffer;

	while(true)
	{
		read_connection_fds = connection_pool;
		int n_select = select(fd_max + 1, &read_connection_fds, NULL, NULL, NULL);

		if (n_select < 0)
		{
			puts("Select Error");
			exit(1);
		}
		for (int i = 0; i <= fd_max; i++)
		{
			if (FD_ISSET(i, &read_connection_fds))
			{
				if (i == s_socket->getSocketFd())
				{
					int connection_fd = s_socket->accept_connection();

					if (connection_fd < 0)
					{
						print_error_message(connection_fd);
						exit(1);
					}

					FD_SET(connection_fd, &connection_pool);
					if (connection_fd > fd_max)
						fd_max = connection_fd;
				}
				else
				{
					buffer_length = s_socket->receive_data(i, &buffer);
					printf("[Listening thread]\t Received %d Bytes\n", buffer_length);

					/*for(int j = 0; j < buffer_length; j++) printf("%d ", buffer[j]);
					putchar('\n');*/

					if (buffer_length <= 0)
					{
						s_socket->close_connection(i);
						FD_CLR(i, &connection_pool);
					}
					else
					{
						char messageType = 0;
						ABSMessage* rcvd_message = NULL;

						memcpy(&messageType, buffer, sizeof (char));
						printf("[Listening thread]\t Message Type: %d\n", messageType);

						switch (messageType)
						{
							case MSG_PEER_INIT:
								rcvd_message = new PeerInitMessage();
								rcvd_message->deserialize(buffer, buffer_length);
								//rcvd_message->message_print_dump();
								break;
                                                                
							case MSG_PLEXUS_GET:
								rcvd_message = new MessageGET();
								rcvd_message->deserialize(buffer, buffer_length);
								//rcvd_message->message_print_dump();
								break;

							case MSG_PLEXUS_GET_REPLY:
								rcvd_message = new MessageGET_REPLY();
								rcvd_message->deserialize(buffer, buffer_length);
								//rcvd_message->message_print_dump();
								break;

                                                    case MSG_PLEXUS_PUT:
								rcvd_message = new MessagePUT();
								rcvd_message->deserialize(buffer, buffer_length);
								//rcvd_message->message_print_dump();
								break;
						}

						if(rcvd_message != NULL)
						{
							((PlexusProtocol*) plexus)->addToIncomingQueue(rcvd_message);
							printf("[Listening thread]\t Added a %d message to the incoming queue\n", rcvd_message->getMessageType());
						}

						delete[] buffer;
					}
				}
			}
		}
	}
}

void *forwarding_thread(void* args)
{
	puts("Starting a forwarding thread");
	char* buffer = NULL;
	int buffer_length;
	ABSMessage* message = NULL;

	while(true)
	{
		message = ((PlexusProtocol*) plexus)->getOutgoingQueueFront();
		ClientSocket* c_socket = new ClientSocket(message->getDestHost(), message->getDestPort());
		c_socket->connect_to_server();

		printf("[Forwarding Thread:]\tForwarding a %d message to %s:%d\n", message->getMessageType(), message->getDestHost().c_str(), message->getDestPort());

		buffer = message->serialize(&buffer_length);
		c_socket->send_data(buffer, buffer_length);

		delete message;
		delete[] buffer;
	}
}

void *processing_thread(void* args)
{
	puts("Starting a processing thread");
	ABSMessage* message = NULL;
	while(true)
	{
		message = ((PlexusProtocol*) plexus)->getIncomingQueueFront();
		printf("[Processing Thread:]\t pulled a %d type message from the incoming queue\n", message->getMessageType());
		bool forward = plexus->getMessageProcessor()->processMessage(message);
		if(forward)
		{
			printf("[Processing Thread:]\t pushed a %d type message for forwarding\n", message->getMessageType());
			((PlexusProtocol*)plexus)->addToOutgoingQueue(message);
		}
	}
}
