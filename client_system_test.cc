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
#include "plnode/message/p2p/message_put.h"
#include <stdlib.h>

int main(int argc, char* argv[])
{
	//char* server_name = argv[1];
	//int server_port = atoi(argv[2]);

	/*Peer* this_peer = new Peer();
	ABSProtocol* plexus = new PlexusProtocol();
	plexus->setContainerPeer(this_peer);*/

	BuildTree tree("hostlist");
	tree.execute();
	tree.print();

	int n = tree.getTreeSize();
	int retCode = 0;
	ClientSocket* c_socket;

	for(int i = 0; i < n; i++)
	{
		HostAddress address = tree.getHostAddress(i);
		printf("%s %d\n", address.GetHostName().c_str(), address.GetHostPort());

		c_socket = new ClientSocket(address.GetHostName(), address.GetHostPort());
		retCode = c_socket->connect_to_server();

		if(retCode < 0)
		{
			print_error_message(retCode);
			continue;
		}

		PeerInitMessage* pInit = new PeerInitMessage();

		LookupTable <OverlayID, HostAddress> rt = tree.getRoutingTablePtr(i);

		rt.reset_iterator();
		/*while(rt.hasMoreKey())
		{
			OverlayID key = rt.getNextKey();
			HostAddress val;
			rt.lookup(key, &val);
			printf("%d %s %d\n", key.GetOverlay_id(), val.GetHostName().c_str(), val.GetHostPort());
		}*/

		pInit->setOID(tree.getOverlayID(i));
		pInit->setNPeers(n);
		pInit->setRoutingTable(tree.getRoutingTablePtr(i));
		char* buffer;
		int buffer_length = 0;
		buffer = pInit->serialize(&buffer_length);

		retCode = c_socket->send_data(buffer, buffer_length);
		if(retCode < 0)
			print_error_message(retCode);
		//else printf("Sent %d Bytes", retCode);
		c_socket->close_socket();
		delete c_socket;
		delete pInit;
	}

	string name_to_publish = "11";
	int hash_name_to_publish = atoi(name_to_publish.c_str());
	HostAddress my_address;
	my_address.SetHostName("localhost");
	my_address.SetHostPort(100);

	MessagePUT* put_msg = new MessagePUT();
	put_msg->SetDeviceName(name_to_publish);
	put_msg->SetHostAddress(my_address);

	OverlayID oID(hash_name_to_publish);
	put_msg->setOID(oID);

	int randHost = rand() % n;
	int ttl = (int)floor(log10(n) / log(2.0)) + 2;

	put_msg->setDestHost(tree.getHostAddress(randHost).GetHostName().c_str());
	put_msg->setDestPort(tree.getHostAddress(randHost).GetHostPort());
	put_msg->setOverlayTtl(ttl);
	//put_msg->message_print_dump();

	ClientSocket cSocket(tree.getHostAddress(randHost).GetHostName(), tree.getHostAddress(randHost).GetHostPort());
	cSocket.connect_to_server();
	int buffer_len = 0;
	char* buffer = put_msg->serialize(&buffer_len);

	/*for(int k = 0; k < buffer_len; k++) printf(" %x", buffer[k]);
	putchar('\n');*/
	cSocket.send_data(buffer, buffer_len);
	cSocket.close_socket();

	name_to_publish = "4000";
	hash_name_to_publish = atoi(name_to_publish.c_str());

	put_msg->SetDeviceName(name_to_publish);
	oID.SetOverlay_id(hash_name_to_publish);
	put_msg->setOID(oID);
	delete[] buffer;
	buffer = put_msg->serialize(&buffer_len);
	cSocket.connect_to_server();
	cSocket.send_data(buffer, buffer_len);
	cSocket.close_socket();

	MessageGET* get_msg = new MessageGET();
	get_msg->SetDeviceName(name_to_publish);
	OverlayID id(hash_name_to_publish);
	get_msg->setOID(id);
	randHost = 2;
	ttl = (int)floor(log10(n) / log(2.0)) + 2;
	get_msg->setDestHost(tree.getHostAddress(randHost).GetHostName().c_str());
	get_msg->setDestPort(tree.getHostAddress(randHost).GetHostPort());
	get_msg->setOverlayTtl(ttl);
	cSocket.setServerHostName(tree.getHostAddress(randHost).GetHostName());
	cSocket.setServerPortNumber(tree.getHostAddress(randHost).GetHostPort());
	cSocket.connect_to_server();
	buffer_len = 0;
	buffer = get_msg->serialize(&buffer_len);
	cSocket.send_data(buffer, buffer_len);
	cSocket.close_socket();

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


