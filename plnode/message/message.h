/*
 * message.h
 *
 *  Created on: 2012-11-21
 *      Author: sr2chowd
 */

#ifndef MESSAGE_H_
#define MESSAGE_H_

#include <string>
#include <stdio.h>
#include "../ds/overlay_id.h"
#include "../ds/GlobalData.h"

using namespace std;

//Message Types
#define MSG_PEER_CONFIG					1
#define MSG_PEER_START					2
#define MSG_PEER_CHANGE_STATUS			3
#define MSG_PEER_INIT					4
#define MSG_PEER_INITIATE_GET			5
#define MSG_DYN_CHANGE_STATUS			11
#define MSG_DYN_CONFIG					12
#define MSG_LOOKUP_CHANGE_STATUS		21
#define MSG_GENERATE_NAME				31
#define	MSG_PLEXUS_PUT					41
#define MSG_PLEXUS_GET					42
#define MSG_PLEXUS_GET_REPLY			43

class ABSMessage
{
	static int sequence_no_generator;

protected:
	unsigned char messageType;
	unsigned int sequence_no;
	string dest_host;
	int dest_port;
	string source_host;
	int source_port;
	unsigned char overlay_hops;
	unsigned char overlay_ttl;
	OverlayID dst_oid;
	OverlayID src_oid;

	size_t getBaseSize()
	{
		size_t size = sizeof(char) * 3 + sizeof(int) * 5
				+ sizeof(char) * (dest_host.size() + source_host.size())
				+ sizeof(OverlayID) * 2;
		return size;
	}

public:
	ABSMessage()
	{
		sequence_no = sequence_no_generator++;
		overlay_hops = 0;
		dest_host = "";
		source_host = "";
        calculateOverlayTTL(GlobalData::network_size);
	}

	ABSMessage(unsigned char messageType, string source_host, int source_port, string dest_host, int dest_port
			, OverlayID src_oid, OverlayID dst_oid): messageType(messageType), source_host(source_host), source_port(source_port),
					dest_host(dest_host), dest_port(dest_port), src_oid(src_oid), dst_oid(dst_oid)
	{
		sequence_no = sequence_no_generator++;
		overlay_hops = 0;
		dest_host = "";
		source_host = "";
        calculateOverlayTTL(GlobalData::network_size);
	}

	virtual ~ABSMessage()
	{
		;
	}



	virtual char* serialize(int* serialize_length)
	{
		char* buffer;
		*serialize_length = getBaseSize();
		buffer = new char[*serialize_length];

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

		memcpy(buffer + offset, (char*)(&dst_oid), sizeof(OverlayID)); offset += sizeof(OverlayID);
		memcpy(buffer + offset, (char*)(&src_oid), sizeof(OverlayID)); offset += sizeof(OverlayID);


		return buffer;
	}

	virtual ABSMessage* deserialize(char* buffer, int buffer_len)
	{
		int offset = 0;
		memcpy(&messageType, buffer + offset, sizeof(char)); offset += sizeof(char); //printf("offset = %d\n", offset);
		memcpy(&sequence_no, buffer + offset, sizeof(int)); offset += sizeof(int); //printf("offset = %d\n", offset);

		int destHostLength = 0;
		memcpy(&destHostLength, buffer + offset, sizeof(int)); offset += sizeof(int); //printf("offset = %d\n", offset);
		dest_host = "";
		//printf("DH Length : %d\n", destHostLength);
		for(int i = 0; i < destHostLength; i++)
		{
			char ch;
			memcpy(&ch, buffer + offset, sizeof(char));
			offset += sizeof(char); //printf("offset = %d\n", offset);
			dest_host += ch;
		}
		memcpy(&dest_port, buffer + offset, sizeof(int)); offset += sizeof(int); //printf("offset = %d\n", offset);

		int sourceHostLength = 0;
		memcpy(&sourceHostLength, buffer + offset, sizeof(int)); offset += sizeof(int); //printf("offset = %d\n", offset);
		source_host = "";
		for(int i = 0; i < sourceHostLength; i++)
		{
			char ch;
			memcpy(&ch, buffer + offset, sizeof(char));
			offset += sizeof(char); //printf("offset = %d\n", offset);
			source_host += ch;
		}
		memcpy(&source_port, buffer + offset, sizeof(int)); offset += sizeof(int); //printf("offset = %d\n", offset);

		memcpy(&overlay_hops, buffer + offset, sizeof(char)); offset += sizeof(char); //printf("offset = %d\n", offset);
		memcpy(&overlay_ttl, buffer + offset, sizeof(char)); offset += sizeof(char); //printf("offset = %d\n", offset);

		memcpy(&dst_oid, buffer + offset, sizeof(OverlayID)); offset += sizeof(OverlayID); //printf("offset = %d\n", offset);
		memcpy(&src_oid, buffer + offset, sizeof(OverlayID)); offset += sizeof(OverlayID); //printf("offset = %d\n", offset);
	}

	virtual void message_print_dump()
	{
		printf("Sequence no. %d\n", sequence_no);
		printf("Message Type. %d\n", messageType);
		printf("Destination host %s\n", dest_host.c_str());
		printf("Destination Port %d\n", dest_port);
		printf("Source Host %s\n", source_host.c_str());
		printf("Source Port %d\n", source_port);
		printf("Source Overlay ID: %d\n", src_oid.GetOverlay_id());
		printf("Destination Overlay ID: %d\n", dst_oid.GetOverlay_id());
		printf("Overlay Hops %d\n", overlay_hops);
		printf("Overlay TTL %d\n", overlay_ttl);
	}

	string getDestHost()
	{
		return dest_host;
	}

	/*void setDestHost(const string& destHost)
	 {
	 dest_host = destHost;
	 }*/

	void setDestHost(const char* destHost)
	{
		dest_host = destHost;
	}

	int getDestPort()
	{
		return dest_port;
	}

	void setDestPort(const int destPort)
	{
		dest_port = destPort;
	}

	unsigned char getOverlayHops()
	{
		return overlay_hops;
	}

	void setOverlayHops(unsigned char overlayHops)
	{
		overlay_hops = overlayHops;
	}

	unsigned char getOverlayTtl()
	{
		return overlay_ttl;
	}

	void setOverlayTtl(unsigned char overlayTtl)
	{
		overlay_ttl = overlayTtl;
	}

	void decrementOverlayTtl()
	{
		overlay_ttl--;
	}

	void incrementOverlayHops()
	{
		overlay_hops++;
	}

	string getSourceHost()
	{
		return source_host;
	}

	void setSourceHost(const string& sourceHost)
	{
		source_host = sourceHost;
	}

	void setSourceHost(const char* sourceHost)
	{
		source_host = sourceHost;
	}

	int getSourcePort()
	{
		return source_port;
	}

	void setSourcePort(int sourcePort)
	{
		source_port = sourcePort;
	}

	int getSequenceNo()
	{
		return sequence_no;
	}

	void setMessageType(unsigned char messageType)
	{
		this->messageType = messageType;
	}

	unsigned char getMessageType() const
	{
		return messageType;
	}

	void setSrcOid(OverlayID oID)
	{
		this->src_oid = oID;
	}

	OverlayID getSrcOid() const
	{
		return src_oid;
	}

	void setDstOid(OverlayID oID)
	{
		this->dst_oid = oID;
	}

	OverlayID getDstOid() const
	{
		return dst_oid;
	}
        
	void calculateOverlayTTL(int networkSize)
	{
		overlay_ttl = 10;
		//overlay_ttl = ceil(log2(n)) + 2;//(int) floor(log10(n) / log(2.0)) + 2;
	}
};

int ABSMessage::sequence_no_generator;

#endif /* MESSAGE_H */
