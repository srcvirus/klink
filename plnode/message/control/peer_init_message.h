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

class PeerInitMessage: public ABSMessage
{
	LookupTable<OverlayID, HostAddress> routing_table;

public:
	virtual char* serialize(int* serialize_length)
	{
		char* buffer = new char[sizeof(PeerInitMessage)];
		memcpy(buffer, (char*)(this), sizeof(PeerInitMessage));
		*serialize_length = sizeof(PeerInitMessage);
		return buffer;
	}

	virtual ABSMessage* deserialize(char* buffer, int buffer_length)
	{
		memcpy(this, buffer, buffer_length);
		return this;
	}
};



#endif /* PEER_INIT_MESSAGE_H_ */
