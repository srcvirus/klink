/*
 * client_system_test.cc
 *
 *  Created on: 2012-12-05
 *      Author: sr2chowd
 */
#include "communication/client_socket.h"
#include "plnode/message/p2p/message_get.h"
#include "communication/error_code.h"
#include <stdlib.h>

int main(int argc, char* argv[])
{
	char* server_name = argv[1];
	int server_port = atoi(argv[2]);
	ClientSocket* c_socket = new ClientSocket(server_name, server_port);
	int error_code = c_socket->connect_to_server();

	if(error_code < 0)
	{
		print_error_message(error_code);
		exit(1);
	}
	puts("Connected to server");
	MessageGET* msg_get = new MessageGET();
	//set member variables
	int buf_length = 20;
	char* buffer = new char[20];
	int bytes_sent = c_socket->send_data(buffer, buf_length);
	printf("%d Bytes Sent\n", bytes_sent);
	c_socket->close_socket();

	delete msg_get;
	delete c_socket;
}


