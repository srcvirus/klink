/*
 * server_test.cc
 *
 *  Created on: 2012-11-24
 *      Author: sr2chowd
 */

#include "../communication/server_socket.h"

int main()
{
	ServerSocket s_socket(20000);
	s_socket.print_socket_info();
	s_socket.listen_to_socket();

	return 0;
}


