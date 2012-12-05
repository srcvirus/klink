/* 
 * File:   plexus_protocol.h
 * Author: mfbari
 *
 * Created on November 29, 2012, 3:10 PM
 */

#ifndef PLEXUS_PROTOCOL_H
#define	PLEXUS_PROTOCOL_H

#include <cstring>
#include <queue>
#include "../protocol.h"
#include "../../ds/cache.h"
#include "../../message/message_processor.h"
#include "../../message/p2p/message_get.h"
#include "../../message/p2p/message_put.h"
#include "../../message/p2p/message_get_reply.h"

using namespace std;

class ABSProtocol;

class PlexusProtocol: public ABSProtocol
{
	queue <ABSMessage*> incoming_message_queue;
	queue <ABSMessage*> outgoing_message_queue;

public:

	PlexusProtocol() :
			ABSProtocol()
	{
		this->routing_table = new LookupTable<OverlayID, HostAddress>();
		this->index_table = new LookupTable<string, OverlayID>();
	}

	PlexusProtocol(LookupTable<OverlayID, HostAddress>* routing_table,
			LookupTable<string, OverlayID>* index_table,
			Cache *cache,
			MessageProcessor* msgProcessor,
			Peer* container) :
			ABSProtocol(routing_table, index_table, cache, msgProcessor, container)
	{
		this->msgProcessor->setContainerProtocol(this);
	}

	bool processMessage(ABSMessage *message)
	{
		return msgProcessor->processMessage(message);
	}

	void initiate_join()
	{
	}

	void process_join()
	{
	}

	void forward(const ABSMessage* msg)
	{
		int maxLengthMatch = 0, currentMatchLength;
		OverlayID idWithLongestMatch;
		//search in the RT
		routing_table->reset_iterator();
		while (routing_table->hasMoreKey())
		{
			OverlayID oid = routing_table->getNextKey();
			currentMatchLength = msg->getOID().GetMatchedPrefixLength(oid);
			if (currentMatchLength > maxLengthMatch)
			{
				maxLengthMatch = currentMatchLength;
				idWithLongestMatch = msg->getOID();
			}
		}
		//search in the CAche
		cache->reset_iterator();
		while (cache->has_next())
		{
			DLLNode *node = cache->get_next();
			OverlayID *id = node->key;
			currentMatchLength = msg->getOID().GetMatchedPrefixLength(*id);
			if (currentMatchLength > maxLengthMatch)
			{
				maxLengthMatch = currentMatchLength;
				idWithLongestMatch = msg->getOID();
			}
		}

		//push in Q with idWithLongestMatchPlexusProtocol
	}

	void get(string name)
	{
		MessageGET *msg = new MessageGET();
		msg->SetDeviceName(name);
		//push in Q
	}

	void put(string name, HostAddress hostAddress)
	{
		MessagePUT *msg = new MessagePUT();
		msg->SetDeviceName(name);
		msg->SetHostAddress(hostAddress);
		//push message in Q
	}

	void rejoin()
	{
	}

	void addToIncomingQueue(ABSMessage* message)
	{
		incoming_message_queue.push(message);
	}

	bool isIncomingQueueEmpty()
	{
		return incoming_message_queue.empty();
	}

	ABSMessage* getIncomingQueueFront()
	{
		ABSMessage* ret = incoming_message_queue.front();
		incoming_message_queue.pop();
		return ret;
	}

	void addToOutgoingQueue(ABSMessage* message)
	{
		outgoing_message_queue.push(message);
	}

	bool isOutgoingQueueEmpty()
	{
		return outgoing_message_queue.empty();
	}

	ABSMessage* getOutgoingQueueFront()
	{
		ABSMessage* ret = outgoing_message_queue.front();
		outgoing_message_queue.pop();
		return ret;
	}

	~PlexusProtocol()
	{
	}
};

#endif	/* PLEXUS_PROTOCOL_H */

