/*
 * peer_init_message.h
 *
 *  Created on: 2012-12-06
 *      Author: sr2chowd
 */

#ifndef PEER_INIT_MESSAGE_H_
#define PEER_INIT_MESSAGE_H_

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

public:
	PeerInitMessage()
	{
		messageType = MSG_PEER_INIT;
	}
	void setRoutingTable(LookupTable <OverlayID, HostAddress>& r_table){ routing_table = r_table; }
	LookupTable<OverlayID, HostAddress>& getRoutingTable(){	return routing_table; }

	virtual char* serialize(int* serialize_length)
	{
		*serialize_length = sizeof(char) + sizeof(int) * 6 + sizeof(char) * dest_host.size() + sizeof(char) * source_host.size() + 2 * sizeof(char) + sizeof(OverlayID);
		routing_table.reset_iterator();
		while(routing_table.hasMoreKey())
		{
			OverlayID key = routing_table.getNextKey();
			HostAddress value;
			routing_table.lookup(key, &value);
			*serialize_length += sizeof(OverlayID) + sizeof(int) * 2;
			*serialize_length += sizeof(char) * value.GetHostName().size();
		}

		char* buffer = new char[*serialize_length];
		int offset = 0;
		int destHostLength = dest_host.size();
		int sourceHostLength = source_host.size();

		memcpy(buffer + offset, (char*)(&messageType), sizeof(char)); offset += sizeof(char);
		memcpy(buffer + offset, (char*)(&sequence_no), sizeof(int)); offset += sizeof(int);

		memcpy(buffer + offset, (char*)(&destHostLength), sizeof(int)); offset += sizeof(int);
		for(int i = 0; i < destHostLength; i++)
		{
			char ch = dest_host[i];
			memcpy(buffer + offset, (char*)(&ch), sizeof(char));
			offset += sizeof(char);
		}
		memcpy(buffer + offset, (char*)(&dest_port), sizeof(int)); offset += sizeof(int);

		memcpy(buffer + offset, (char*)(&sourceHostLength), sizeof(int)); offset += sizeof(int);
		for(int i = 0; i < sourceHostLength; i++)
		{
			char ch = source_host[i];
			memcpy(buffer + offset, (char*)(&ch), sizeof(char));
			offset += sizeof(char);
		}
		memcpy(buffer + offset, (char*)(&source_port), sizeof(int)); offset += sizeof(int);

		memcpy(buffer + offset, (char*)(&overlay_hops), sizeof(char)); offset += sizeof(char);
		memcpy(buffer + offset, (char*)(&overlay_ttl), sizeof(char)); offset += sizeof(char);
		memcpy(buffer + offset, (char*)(&oID), sizeof(OverlayID)); offset += sizeof(OverlayID);

		int routingTableSize = routing_table.size();
		memcpy(buffer + offset, (char*)(&routingTableSize), sizeof(int)); offset += sizeof(int);

		routing_table.reset_iterator();
		while(routing_table.hasMoreKey())
		{
			OverlayID key = routing_table.getNextKey();
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
		//memcpy(buffer, (char*)(this), sizeof(PeerInitMessage));
		*serialize_length = sizeof(PeerInitMessage);
		return buffer;
	}

	virtual ABSMessage* deserialize(char* buffer, int buffer_length)
	{
		int offset = 0;
		int destHostLength, sourceHostLength;
		routing_table.clear();

		memcpy(&messageType, buffer + offset, sizeof(char)); offset += sizeof(char); printf("offset = %d\n", offset);
		memcpy(&sequence_no, buffer + offset, sizeof(int)); offset += sizeof(int); printf("offset = %d\n", offset);
		memcpy(&destHostLength, buffer + offset, sizeof(int)); offset += sizeof(int); printf("offset = %d\n", offset);
		dest_host = "";
		printf("DH Length : %d\n", destHostLength);
		for(int i = 0; i < destHostLength; i++)
		{
			char ch;
			memcpy(&ch, buffer + offset, sizeof(char));
			offset += sizeof(char); printf("offset = %d\n", offset);
			dest_host += ch;
		}
		memcpy(&dest_port, buffer + offset, sizeof(int)); offset += sizeof(int); printf("offset = %d\n", offset);
		memcpy(&sourceHostLength, buffer + offset, sizeof(int)); offset += sizeof(int); printf("offset = %d\n", offset);
		source_host = "";
		for(int i = 0; i < sourceHostLength; i++)
		{
			char ch;
			memcpy(&ch, buffer + offset, sizeof(char));
			offset += sizeof(char); printf("offset = %d\n", offset);
			source_host += ch;
		}
		memcpy(&source_port, buffer + offset, sizeof(int)); offset += sizeof(int); printf("offset = %d\n", offset);
		memcpy(&overlay_hops, buffer + offset, sizeof(char)); offset += sizeof(char); printf("offset = %d\n", offset);
		memcpy(&overlay_ttl, buffer + offset, sizeof(char)); offset += sizeof(char); printf("offset = %d\n", offset);
		memcpy(&oID, buffer + offset, sizeof(OverlayID)); offset += sizeof(OverlayID); printf("offset = %d\n", offset);

		int routingTableSize;
		memcpy(&routingTableSize, buffer + offset, sizeof(int)); offset += sizeof(int); printf("offset = %d\n", offset);

		for(int i = 0; i < routingTableSize; i++)
		{
			OverlayID key;
			HostAddress value;
			int hostNameLength;
			string hostname;
			int hostport;

			memcpy(&key, buffer + offset, sizeof(OverlayID)); offset += sizeof(OverlayID); printf("offset = %d\n", offset);
			memcpy(&hostNameLength, buffer + offset, sizeof(int)); offset += sizeof(int); printf("offset = %d\n", offset);
			for(int i = 0; i < hostNameLength; i++)
			{
				char ch;
				memcpy(&ch, buffer + offset, sizeof(char)); offset += sizeof(char); printf("offset = %d\n", offset);
				hostname += ch;
			}
			memcpy(&hostport, buffer + offset, sizeof(int)); offset += sizeof(int); printf("offset = %d\n", offset);
			value.SetHostName(hostname);
			value.SetHostPort(hostport);
			routing_table.add(key, value);
		}
	//	memcpy(&this->messageType, buffer + offset, sizeof(int));
	//	memcpy(this, buffer, buffer_length);
		return this;
	}

	virtual void message_print_dump()
	{
		puts("<<<<<<<<<<<<<<<<<<<<>>>>>>>>>>>>>>>>>>>>");
		printf("Init Message\n");
		ABSMessage::message_print_dump();
		routing_table.reset_iterator();
		while(routing_table.hasMoreKey())
		{
			OverlayID key = routing_table.getNextKey();
			HostAddress value;
			routing_table.lookup(key, &value);
			printf("Overlay ID = %d, Hostname = %s, Host Port = %d\n", key.GetOverlay_id(), value.GetHostName().c_str(), value.GetHostPort());
		}
		puts("<<<<<<<<<<<<<<<<<<<<>>>>>>>>>>>>>>>>>>>>");
	}
};



#endif /* PEER_INIT_MESSAGE_H_ */
