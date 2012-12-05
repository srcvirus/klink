/*
 * client_system_test.cc
 *
 *  Created on: 2012-12-05
 *      Author: sr2chowd
 */
#include "communication/client_socket.h"
#include "plnode/message/p2p/message_get.h"
#include <stdlib.h>

int main(int argc, char* argv[])
{
	char* server_name = argv[1];
	int server_port = atoi(argv[2]);
	ClientSocket* c_socket = new ClientSocket(server_name, server_port);

	MessageGET* msg_get = new MessageGET();
	//set member variables
	int buf_length = 0;
	char* buffer = msg_get->serialize(&buf_length);
	c_socket->send_data(buffer, buf_length);
	c_socket->close_socket();

	delete msg_get;
	delete c_socket;
}


