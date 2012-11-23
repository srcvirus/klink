/*
 * message.h
 *
 *  Created on: 2012-11-21
 *      Author: sr2chowd
 */

#ifndef MESSAGE_H_
#define MESSAGE_H_

#include <string>

using namespace std;

//Message Types
#define MSG_PEER_CONFIG					1
#define MSG_PEER_START					2
#define MSG_PEER_CHANGE_STATUS				3
#define MSG_DYN_CHANGE_STATUS				11
#define MSG_DYN_CONFIG					12
#define MSG_LOOKUP_CHANGE_STATUS			21
#define MSG_GENERATE_NAME				31
#define	MSG_PLEXUS_PUT					41
#define MSG_PLEXUS_GET					42

class ABSMessage
{
protected:
	unsigned int sequence_no;
	unsigned char messageType;
	string dest_host;
	int dest_port;
	string source_host;
	int source_port;
	unsigned char overlay_hops;
	unsigned char overlay_ttl;
	
	static int sequence_no_generator = 0;

public:
	ABSMessage(){ sequence_no = ABSMessage.sequence_no_generator++; }
	virtual ~ABSMessage();

	virtual char* serialize(int* serialize_length) = 0;

	virtual ABSMessage* deserialize(char* buffer, int buffer_len) = 0;

	virtual void message_print_dump(){;}

	string getDestHost()
	{
		return dest_host;
	}

	void setDestHost(const string& destHost)
	{
		dest_host = destHost;
	}

	void setDestHost(const char* destHost)
	{
		dest_host = destHost;
	}

	int getDestPort()
	{
		return dest_port;
	}

	void setDestPort(int destPort)
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
};

#endif /* MESSAGE_H_ */
