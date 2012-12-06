/*
 * client_system_test.cc
 *
 *  Created on: 2012-12-05
 *      Author: sr2chowd
 */
#include "communication/client_socket.h"
#include "plnode/message/p2p/message_get.h"
#include "communication/error_code.h"
#include "monitor/tree/BuildTree.h"
#include "plnode/message/control/peer_init_message.h"

#include <stdlib.h>

int main(int argc, char* argv[])
{
	char* server_name = argv[1];
	int server_port = atoi(argv[2]);

	/*Peer* this_peer = new Peer();
	ABSProtocol* plexus = new PlexusProtocol();
	plexus->setContainerPeer(this_peer);*/

	BuildTree tree("hostlist");
	tree.execute();
	tree.print();

	int n = tree.getTreeSize();

	for(int i = 0; i < n; i++)
	{
		HostAddress address = tree.getHostAddress(i);
		printf("%s %d\n", address.GetHostName().c_str(), address.GetHostPort());
		ClientSocket c_socket(address.GetHostName(), address.GetHostPort());
//		c_socket.connect_to_server();

		PeerInitMessage* pInit = new PeerInitMessage();
		LookupTable <OverlayID, HostAddress> rt = tree.getRoutingTablePtr(i);

		rt.reset_iterator();
		while(rt.hasMoreKey())
		{
			OverlayID key = rt.getNextKey();
			HostAddress val;
			rt.lookup(key, &val);
			printf("%d %s %d\n", key.GetOverlay_id(), val.GetHostName().c_str(), val.GetHostPort());
		}

		pInit->setRoutingTable(tree.getRoutingTablePtr(i));
		char* buffer;
		int buffer_length = 0;
		buffer = pInit->serialize(&buffer_length);

//		c_socket.send_data(buffer, buffer_length);
//		c_socket.close_socket();
	}
	//plexus->put("name");
	//plexus->get("name");

/*	ClientSocket* c_socket = new ClientSocket(server_name, server_port);
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
	delete c_socket;*/
}


