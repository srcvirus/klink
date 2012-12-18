/* 
 * File:   plexus_message_processor.h
 * Author: mfbari
 *
 * Created on November 29, 2012, 2:45 PM
 */

#ifndef PLEXUS_MESSAGE_PROCESSOR_H
#define	PLEXUS_MESSAGE_PROCESSOR_H

#include "../../message/message.h"
#include "../../message/p2p/message_get.h"
#include "../../message/message_processor.h"
#include "../../message/control/peer_init_message.h"
#include "../../message/control/peer_initiate_get.h"
#include "../../message/control/peer_initiate_put.h"
#include "../../message/p2p/message_put.h"
#include "../../message/p2p/message_get_reply.h"
#include "../../message/control/peer_config_msg.h"
#include "../protocol.h"
#include "../plexus/plexus_protocol.h"
#include "../../../common/util.h"
#include "../../ds/cache_insert_endpoint.h"
#include "../../ds/cache_replace_LRU.h"
#include "../../ds/cache.h"
#include "../../peer/peer_status.h"
#include "../../message/control/peer_change_status_message.h"

class PlexusMessageProcessor: public MessageProcessor
{
public:

	void setup(LookupTable<OverlayID, HostAddress>* routing_table,
			LookupTable<string, HostAddress>* index_table)
	{
		MessageProcessor::setup(routing_table, index_table,
				new Cache(new CacheInsertEndpoint(), new CacheReplaceLRU(),
						getContainerProtocol()->getRoutingTable(), 100));
	}

	bool processMessage(ABSMessage* message)
	{
		message->decrementOverlayTtl();

		PlexusProtocol* plexus = (PlexusProtocol*) container_protocol;
		Peer* container_peer = container_protocol->getContainerPeer();

		bool forward = plexus->setNextHop(message);

		if (forward)
			return true;

		//PUT
		if (message->getMessageType() == MSG_PLEXUS_PUT)
		{
			MessagePUT* putMsg = (MessagePUT*) message;
			index_table->add(putMsg->GetDeviceName(), putMsg->GetHostAddress());
			puts("PUT Successful");
		} //GET
		else if (message->getMessageType() == MSG_PLEXUS_GET)
		{
			MessageGET *msg = ((MessageGET*) message);
			HostAddress hostAddress;
			if (index_table->lookup(msg->GetDeviceName(), &hostAddress))
			{
				puts("Got it :)");
				MessageGET_REPLY *reply = new MessageGET_REPLY(
						container_peer->getHostName(),
						container_peer->getListenPortNumber(),
						msg->getSourceHost(), msg->getSourcePort(),
						container_peer->getOverlayID(), msg->getDstOid(),
						SUCCESS, hostAddress);

				plexus->addToOutgoingQueue(reply);
				//send message
			} else
			{
				puts("GET Failed");
				MessageGET_REPLY *reply = new MessageGET_REPLY(
						container_peer->getHostName(),
						container_peer->getListenPortNumber(),
						msg->getSourceHost(), msg->getSourcePort(),
						container_peer->getOverlayID(), msg->getDstOid(),
						ERROR_GET_FAILED, hostAddress);
				plexus->addToOutgoingQueue(reply);
			}
		} //GET_REPLY
		else if (message->getMessageType() == MSG_PLEXUS_GET_REPLY)
		{
			MessageGET_REPLY *msg = ((MessageGET_REPLY*) message);
			OverlayID srcID = msg->getSrcOid();
			cache->add(&srcID,
					new HostAddress(msg->getSourceHost(),
							msg->getSourcePort()));
			cache->print();
		} //INIT Message
		else if (message->getMessageType() == MSG_PEER_INIT)
		{
			PeerInitMessage* pInitMsg = (PeerInitMessage*) message;
			container_protocol->setRoutingTable(&pInitMsg->getRoutingTable());
			container_protocol->setIndexTable(new LookupTable <string, HostAddress>());

			container_peer->setNPeers(pInitMsg->getNPeers());
			GlobalData::network_size = pInitMsg->getNPeers();
			container_peer->setOverlayID(pInitMsg->getDstOid());

			container_peer->SetInitRcvd(true);

			container_peer->setPublish_name_range_start(
					pInitMsg->getPublish_name_range_start());
			container_peer->setPublish_name_range_end(
					pInitMsg->getPublish_name_range_end());
			container_peer->setLookup_name_range_start(
					pInitMsg->getLookup_name_range_start());
			container_peer->setLookup_name_range_end(
					pInitMsg->getLookup_name_range_end());

			this->setup(container_protocol->getRoutingTable(),
					container_protocol->getIndexTable());
		}
		else if (message->getMessageType() == MSG_PEER_CONFIG)
		{
			PeerConfigMessage* pConfMsg = (PeerConfigMessage*) message;
			container_peer->setAlpha(pConfMsg->getAlpha());
			container_peer->setK(pConfMsg->getK());
		}
		else if (message->getMessageType() == MSG_PEER_INITIATE_GET)
		{
			PeerInitiateGET* pInitGet = (PeerInitiateGET*) message;
			container_protocol->get(pInitGet->getDeviceName());
		}
		else if (message->getMessageType() == MSG_PEER_INITIATE_PUT)
		{
			PeerInitiatePUT* pInitPut = (PeerInitiatePUT*) message;
			container_protocol->put(pInitPut->getDeviceName(),
					pInitPut->GetHostAddress());
		}
		else if (message->getMessageType() == MSG_PEER_START)
		{
			container_protocol->getContainerPeer()->setStatus(PEER_RUNNING);
		}
		else if (message->getMessageType() == MSG_PEER_CHANGE_STATUS)
		{
			PeerChangeStatusMessage* changeStatusMSG =
					(PeerChangeStatusMessage*) message;
			container_protocol->getContainerPeer()->setStatus(
					changeStatusMSG->getPeer_status());
		}
		return false;
	}
};

#endif	/* PLEXUS_MESSAGE_PROCESSOR_H */

