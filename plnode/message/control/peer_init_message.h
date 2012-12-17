/*
 * peer_init_message.h
 *
 *  Created on: 2012-12-06
 *      Author: sr2chowd
 */

#ifndef PEER_INIT_MESSAGE_H_
#define PEER_INIT_MESSAGE_H_

#include "../../../plnode/ds/host_address.h"
#include "../../../plnode/ds/overlay_id.h"
#include "../../../plnode/ds/lookup_table.h"
#include "../../../plnode/ds/lookup_table_iterator.h"
#include "../message.h"
#include <memory.h>

/*
 * unsigned char messageType;
	unsigned int sequence_no;
	string dest_host;
	int dest_port;
	string source_host;
	int source_port;
	unsigned char overlay_hops;
	unsigned char overlay_ttl;
	OverlayID oID;
 */
class PeerInitMessage: public ABSMessage
{
	LookupTable<OverlayID, HostAddress> routing_table;
	int n_peers, k;
	double alpha;

public:
	PeerInitMessage()
	{
		messageType = MSG_PEER_INIT;
	}
	void setRoutingTable(LookupTable <OverlayID, HostAddress>& r_table){ routing_table = r_table; }
	LookupTable<OverlayID, HostAddress>& getRoutingTable(){	return routing_table; }

	void setNPeers(int n){ n_peers = n; }
	int getNPeers(){ return n_peers; }

	size_t getSize()
	{
		int ret = getBaseSize();
		ret += sizeof(int) * 3;
		ret += sizeof(double);
		LookupTableIterator <OverlayID, HostAddress> rtable_iterator(&routing_table);
		rtable_iterator.reset_iterator();

		while(rtable_iterator.hasMoreKey())
		{
			OverlayID key = rtable_iterator.getNextKey();
			HostAddress value;
			routing_table.lookup(key, &value);
			//printf("<< %d %s %d >>\n", key.GetOverlay_id(), value.GetHostName().c_str(), value.GetHostPort());
			ret += sizeof(OverlayID) + sizeof(int) * 2;
			ret += sizeof(char) * value.GetHostName().size();
		}
		return ret;
	}

	virtual char* serialize(int* serialize_length)
	{
		*serialize_length = getSize();
		LookupTableIterator <OverlayID, HostAddress> rtable_iterator(&routing_table);

		int offset = 0;
		int parent_size = 0;

		char* parent_buffer = ABSMessage::serialize(&parent_size);
		char* buffer = new char[*serialize_length];

		for(int i = 0; i < parent_size; i++) buffer[i] = parent_buffer[i];
		offset = parent_size;

		memcpy(buffer + offset, (char*)(&n_peers), sizeof(int)); offset += sizeof(int);
		memcpy(buffer + offset, (char*)(&k), sizeof(int)); offset += sizeof(int);
		memcpy(buffer + offset, (char*)(&alpha), sizeof(double)); offset += sizeof(double);

		int routingTableSize = routing_table.size();
		memcpy(buffer + offset, (char*)(&routingTableSize), sizeof(int)); offset += sizeof(int);

		rtable_iterator.reset_iterator();

		while(rtable_iterator.hasMoreKey())
		{
			OverlayID key = rtable_iterator.getNextKey();
			HostAddress value;
			routing_table.lookup(key, &value);
			int hostNameLength = value.GetHostName().size();

			memcpy(buffer + offset, (char*)(&key), sizeof(OverlayID)); offset += sizeof(OverlayID);
			memcpy(buffer + offset, (char*)(&hostNameLength), sizeof(int)); offset += sizeof(int);

			for(int i = 0; i < hostNameLength; i++)
			{
				char ch = value.GetHostName()[i];
				memcpy(buffer + offset, (char*)(&ch), sizeof(char));
				offset += sizeof(char);
			}
			int hostport = value.GetHostPort();
			memcpy(buffer + offset, (char*)(&hostport), sizeof(int)); offset += sizeof(int);
		}

		delete[] parent_buffer;

		return buffer;
	}

	virtual ABSMessage* deserialize(char* buffer, int buffer_length)
	{
		int offset = 0;
		routing_table.clear();

		ABSMessage::deserialize(buffer, buffer_length);
		offset = getBaseSize();

		memcpy(&n_peers, buffer + offset, sizeof(int)); offset += sizeof(int);
		memcpy(&k, buffer + offset, sizeof(int)); offset += sizeof(int);
		memcpy(&alpha, buffer + offset, sizeof(double)); offset += sizeof(double);

		int routingTableSize;
		memcpy(&routingTableSize, buffer + offset, sizeof(int)); offset += sizeof(int); //printf("offset = %d\n", offset);

		for(int i = 0; i < routingTableSize; i++)
		{
			OverlayID key;
			HostAddress value;
			int hostNameLength;
			string hostname;
			int hostport;

			memcpy(&key, buffer + offset, sizeof(OverlayID)); offset += sizeof(OverlayID); //printf("offset = %d\n", offset);
			memcpy(&hostNameLength, buffer + offset, sizeof(int)); offset += sizeof(int); //printf("offset = %d\n", offset);
			for(int i = 0; i < hostNameLength; i++)
			{
				char ch;
				memcpy(&ch, buffer + offset, sizeof(char)); offset += sizeof(char); //printf("offset = %d\n", offset);
				hostname += ch;
			}
			memcpy(&hostport, buffer + offset, sizeof(int)); offset += sizeof(int); //printf("offset = %d\n", offset);
			value.SetHostName(hostname);
			value.SetHostPort(hostport);
			routing_table.add(key, value);
		}

		return this;
	}

	virtual void message_print_dump()
	{
		puts("<<<<<<<<<<<<<<<<<<<<>>>>>>>>>>>>>>>>>>>>");
		printf("Init Message\n");
		ABSMessage::message_print_dump();
		LookupTableIterator <OverlayID, HostAddress> rtable_iterator(&routing_table);
		//routing_table.reset_iterator();
		rtable_iterator.reset_iterator();

		while(rtable_iterator.hasMoreKey())
		//while(routing_table.hasMoreKey())
		{
			OverlayID key = rtable_iterator.getNextKey();
			//OverlayID key = routing_table.getNextKey();
			HostAddress value;
			routing_table.lookup(key, &value);
			printf("Overlay ID = %d, Hostname = %s, Host Port = %d\n", key.GetOverlay_id(), value.GetHostName().c_str(), value.GetHostPort());
		}
		puts("<<<<<<<<<<<<<<<<<<<<>>>>>>>>>>>>>>>>>>>>");
	}
};



#endif /* PEER_INIT_MESSAGE_H_ */
