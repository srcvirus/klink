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

using namespace std;

//Message Types
#define MSG_PEER_CONFIG					1
#define MSG_PEER_START					2
#define MSG_PEER_CHANGE_STATUS			3
#define MSG_PEER_INIT					4
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
	OverlayID oID;

	size_t getBaseSize()
	{
		size_t size = sizeof(char) * 3 + sizeof(int) * 5 + sizeof(char) * (dest_host.size() + source_host.size()) + sizeof(OverlayID);
		return size;
	}

public:
	ABSMessage()
	{
		sequence_no = sequence_no_generator++;
		overlay_hops = 0;
		dest_host = "";
		source_host = "";
	}


	virtual ~ABSMessage(){;}

	virtual char* serialize(int* serialize_length) = 0;

	virtual ABSMessage* deserialize(char* buffer, int buffer_len) = 0;

	virtual void message_print_dump()
	{
		printf("Sequence no. %d\n", sequence_no);
		printf("Message Type. %d\n", messageType);
		printf("Destination host %s\n", dest_host.c_str());
		printf("Destination Port %d\n", dest_port);
		printf("Source Host %s\n", source_host.c_str());
		printf("Source Port %d\n", source_port);
		printf("Overlay ID: %d\n", oID.GetOverlay_id());
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

        void setMessageType(unsigned char messageType) {
            this->messageType = messageType;
        }

        unsigned char getMessageType() const {
            return messageType;
        }

        void setOID(OverlayID oID) {
            this->oID = oID;
        }

        OverlayID getOID() const {
            return oID;
        }
};

int ABSMessage::sequence_no_generator;

#endif /* MESSAGE_H_ */
