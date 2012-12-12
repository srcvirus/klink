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

Peer* this_peer;
ABSProtocol* plexus;
fd_set connection_pool;
fd_set read_connection_fds;
ServerSocket* s_socket = NULL;
int fd_max;

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

void process_and_forward(ABSMessage* rcvd_message) {
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
}

int main(int argc, char* argv[]) {
    
    //int port = atoi(argv[1]);
    int error_code;

    //this_peer = new Peer(port);
    this_peer = new Peer(GlobalData::host_file_name);
    plexus = new PlexusProtocol();

    PlexusMessageProcessor* msg_processor = new PlexusMessageProcessor();

    msg_processor->setContainerProtocol(plexus);
    plexus->setMessageProcess(msg_processor);

    plexus->setContainerPeer(this_peer);
    this_peer->setProtocol(plexus);

    FD_ZERO(&connection_pool);
    FD_ZERO(&read_connection_fds);

    s_socket = this_peer->getServerSocket();
    error_code = s_socket->init_connection();
    FD_SET(s_socket->getSocketFd(), &connection_pool);

    if (error_code < 0) {
        print_error_message(error_code);
        exit(1);
    }

    fd_max = s_socket->getSocketFd();
    s_socket->print_socket_info();

    char* buffer;
    int buffer_length;

    while (true) {
        read_connection_fds = connection_pool;

        //puts("Waiting");
        int n_select = select(fd_max + 1, &read_connection_fds, NULL, NULL, NULL);
        //puts("Interrupt");
        if (n_select < 0) {
            puts("Select Error");
            exit(1);
        }
        bool forward;

        for (int i = 0; i <= fd_max; i++) {
            if (FD_ISSET(i, &read_connection_fds)) {
                if (i == s_socket->getSocketFd()) {
                    int connection_fd = s_socket->accept_connection();
                    //puts("Connection Established");

                    if (connection_fd < 0) {
                        print_error_message(connection_fd);
                        exit(1);
                    }
                    FD_SET(connection_fd, &connection_pool);
                    if (connection_fd > fd_max)
                        fd_max = connection_fd;
                } else {
                    buffer_length = s_socket->receive_data(i, &buffer);
                    printf("Received %d Bytes\n", buffer_length);
                    /*for(int j = 0; j < buffer_length; j++) printf("%d ", buffer[j]);
                    putchar('\n');*/

                    //printf("Received %d Bytes\n", buffer_length);
                    if (buffer_length <= 0) {
                        s_socket->close_connection(i);
                        FD_CLR(i, &connection_pool);
                    }//do the processing with the message
                    else {
                        char messageType = 0;
                        ABSMessage* rcvd_message;

                        memcpy(&messageType, buffer, sizeof (char));
                        printf("Message Type: %d\n", messageType);

                        switch (messageType) {
                            case MSG_PEER_INIT:
                                rcvd_message = new PeerInitMessage();
                                rcvd_message->deserialize(buffer, buffer_length);
                                rcvd_message->message_print_dump();
                                ((PlexusProtocol*) plexus)->processMessage(rcvd_message);
                                break;
                            case MSG_PLEXUS_GET:
                                rcvd_message = new MessageGET();
                                rcvd_message->deserialize(buffer, buffer_length);
                                //rcvd_message->message_print_dump();
                                process_and_forward(rcvd_message);
                                /*forward = plexus->getMessageProcessor()->processMessage(rcvd_message);
                                if(forward)
                                {
                                        puts("forwarding");
                                        process_and_forward(rcvd_message);
                                }
                                else puts("stopped");*/
                                break;

                            case MSG_PLEXUS_PUT:
                                rcvd_message = new MessagePUT();
                                rcvd_message->deserialize(buffer, buffer_length);
                                //rcvd_message->message_print_dump();
                                process_and_forward(rcvd_message);
                                break;
                        }
                        delete[] buffer;
                    }
                }
            }
        }
    }

    delete s_socket;
}



