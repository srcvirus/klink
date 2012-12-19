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
#include "plnode/message/control/peer_initiate_get.h"
#include "plnode/message/control/peer_initiate_put.h"
#include "plnode/message/control/peer_start_message.h"
#include "plnode/message/control/peer_change_status_message.h"

#include <cstdlib>
#include <cstdio>
#include <pthread.h>

using namespace std;

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
void *controlling_thread(void*);

int main(int argc, char* argv[])
{
	system_init();
	pthread_t listener, processor, forwarder, controller;

	pthread_create(&listener, NULL, listener_thread, NULL);
	pthread_create(&forwarder, NULL, forwarding_thread, NULL);
	pthread_create(&processor, NULL, processing_thread, NULL);
	//pthread_create(&controller, NULL, controlling_thread, NULL);

	pthread_join(listener, NULL);
	pthread_join(forwarder, NULL);
	pthread_join(processor, NULL);
	//pthread_join(controller, NULL);

	cleanup();
}

void system_init()
{
	int error_code;
	puts("Initializing the System");

	/* creating the peer container */
	this_peer = new Peer(GlobalData::host_file_name.c_str());
	puts("hostname = ");
	puts(this_peer->getHostName().c_str());
	if (this_peer->getListenPortNumber() == -1)
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

	this_peer->setNRetry(2);
	this_peer->setTimeoutSec(5);
	this_peer->setTimeoutMicroSec(500);
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

	while (true)
	{
		read_connection_fds = connection_pool;
		int n_select = select(fd_max + 1, &read_connection_fds, NULL, NULL, NULL);

		if (n_select < 0)
		{
			puts("Select Error");
			printf("%d\n", errno);
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
				} else
				{
					buffer_length = s_socket->receive_data(i, &buffer);
					printf("[Listening thread]\t Received %d Bytes\n",
							buffer_length);

					/*for(int j = 0; j < buffer_length; j++) printf("%d ", buffer[j]);
					 putchar('\n');*/

					if (buffer_length <= 0)
					{
						s_socket->close_connection(i);
						FD_CLR(i, &connection_pool);
					} else
					{
						char messageType = 0;
						ABSMessage* rcvd_message = NULL;

						memcpy(&messageType, buffer, sizeof(char));
						printf("[Listening thread]\t Message Type: %d\n",
								messageType);

						switch (messageType)
						{
						case MSG_PEER_INIT:
							rcvd_message = new PeerInitMessage();
							rcvd_message->deserialize(buffer, buffer_length);
							rcvd_message->message_print_dump();
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

						case MSG_PEER_INITIATE_GET:
							rcvd_message = new PeerInitiateGET();
							rcvd_message->deserialize(buffer, buffer_length);
							break;
						case MSG_PEER_INITIATE_PUT:
							rcvd_message = new PeerInitiatePUT();
							rcvd_message->deserialize(buffer, buffer_length);
							break;
						case MSG_PEER_START:
							rcvd_message = new PeerStartMessage();
							rcvd_message->deserialize(buffer, buffer_length);
							break;
						case MSG_PEER_CHANGE_STATUS:
							rcvd_message = new PeerChangeStatusMessage();
							rcvd_message->deserialize(buffer, buffer_length);
							break;
						}

						if (rcvd_message != NULL)
						{
							((PlexusProtocol*) plexus)->addToIncomingQueue(
									rcvd_message);
							printf(
									"[Listening thread]\t Added a %d message to the incoming queue\n",
									rcvd_message->getMessageType());
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

	while (true)
	{
		message = ((PlexusProtocol*) plexus)->getOutgoingQueueFront();
		message->incrementOverlayHops();

		printf("[Forwarding Thread:]\tForwarding a %d message to %s:%d\n",
				message->getMessageType(), message->getDestHost().c_str(),
				message->getDestPort());

		int retry = 0;
		while(retry < this_peer->getNRetry())
		{
			int error_code = plexus->send_message(message);

			if(error_code == ERROR_CONNECTION_TIMEOUT)
				retry++;
			else
				break;
		}

		delete message;
	}
}

void *processing_thread(void* args)
{
	puts("Starting a processing thread");
	ABSMessage* message = NULL;
	while (true)
	{
		message = ((PlexusProtocol*) plexus)->getIncomingQueueFront();
		printf(
				"[Processing Thread:]\t pulled a %d type message from the incoming queue\n",
				message->getMessageType());

		bool forward = plexus->getMessageProcessor()->processMessage(message);
		if (forward)
		{
			printf("[Processing Thread:]\t pushed a %d type message for forwarding\n",
					message->getMessageType());

			message->getDstOid().printBits();
			printf(" host: %s:%d TTL: %d Hops: %d\n",
					message->getDestHost().c_str(), message->getDestPort(),
					message->getOverlayTtl(), message->getOverlayHops());
			((PlexusProtocol*) plexus)->addToOutgoingQueue(message);
		}
	}
}

void *controlling_thread(void* args)
{
	puts("Starting a controlling thread");

	while (true)
	{
		if (this_peer->IsInitRcvd())
		{
			char buffer[33];

			//publish names
			printf("[Controlling Thread:]\tPublishing name in range %d %d\n",
					this_peer->getPublish_name_range_start(),
					this_peer->getPublish_name_range_end());

			for (int i = this_peer->getPublish_name_range_start();
					i < this_peer->getPublish_name_range_end(); i++)
			{
				HostAddress ha("dummyhost", i);
				//itoa(i, buffer, 10);
				sprintf(buffer, "%d", i);
				printf("[Controlling Thread:]\tPublishing name: %d\n", i);
				this_peer->getProtocol()->put(string(buffer), ha);
				if (i % 3 == 0)
					pthread_yield();
			}
			//lookup names
			printf("[Controlling Thread:]\tLooking up name ...\n");
			for (int i = this_peer->getLookup_name_range_start();
					i < this_peer->getLookup_name_range_end(); i++)
			{
				HostAddress ha("dummyhost", i);
				//itoa(i, buffer, 10);
				sprintf(buffer, "%d", i);
				printf("[Controlling Thread:]\tLooking up name: %d\n", i);
				this_peer->getProtocol()->get(string(buffer));
				if (i % 3 == 0)
					pthread_yield();
			}
			break;
		}
		pthread_yield();
	}
}
