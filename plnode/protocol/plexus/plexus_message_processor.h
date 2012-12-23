/* 
 * File:   plexus_message_processor.h
 * Author: mfbari
 *
 * Created on November 29, 2012, 2:45 PM
 */

#ifndef PLEXUS_MESSAGE_PROCESSOR_H
#define	PLEXUS_MESSAGE_PROCESSOR_H

#include "../../message/message_processor.h"

#include "../../message/message.h"
#include "../../message/p2p/message_get.h"
#include "../../message/p2p/message_put.h"
#include "../../message/p2p/message_get_reply.h"
#include "../../message/p2p/message_put_reply.h"

#include "../../message/control/peer_init_message.h"
#include "../../message/control/peer_initiate_get.h"
#include "../../message/control/peer_initiate_put.h"
#include "../../message/control/peer_config_msg.h"
#include "../../message/control/peer_change_status_message.h"
#include "../../message/control/peer_gen_name_message.h"
#include "../../message/control/peer_dyn_change_status_message.h"

#include "../protocol.h"
#include "../plexus/plexus_protocol.h"

#include "../../../common/util.h"

#include "../../ds/cache.h"
#include "../../ds/cache_insert_endpoint.h"
#include "../../ds/cache_replace_LRU.h"

#include "../../ds/overlay_id.h"

#include "../../peer/peer_status.h"

#include "../../logging/log_entry.h"

class PlexusMessageProcessor: public MessageProcessor
{
public:

	void setup(LookupTable<OverlayID, HostAddress>* routing_table,
			LookupTable<string, HostAddress>* index_table)
	{
		MessageProcessor::setup(routing_table, index_table,
				new Cache(new CacheInsertEndpoint(), new CacheReplaceLRU(),
						container_protocol->getRoutingTable(),
						container_protocol->getContainerPeer()->getOverlayID(),
						container_protocol->getContainerPeer()->getK()));
	}

	bool processMessage(ABSMessage* message)
	{
		if (message->getMessageType() == MSG_PLEXUS_GET
				|| message->getMessageType() == MSG_PLEXUS_PUT)
			message->setProcessingStartT(clock());

		message->decrementOverlayTtl();
		PlexusProtocol* plexus = (PlexusProtocol*) container_protocol;
		Peer* container_peer = container_protocol->getContainerPeer();

		bool forward = plexus->setNextHop2(message);

		if (forward)
		{
			if (message->getMessageType() == MSG_PLEXUS_GET
					|| message->getMessageType() == MSG_PLEXUS_PUT)
				message->setProcessingEndT(clock());
			return true;
		}
		//PUT
		if (message->getMessageType() == MSG_PLEXUS_PUT)
		{
			container_peer->incrementPut_processed();
            MessagePUT* putMsg = (MessagePUT*) message;
			puts("Adding to index table");
			index_table->add(putMsg->GetDeviceName(), putMsg->GetHostAddress());
			putMsg->setProcessingEndT(clock());
			putMsg->updateStatistics();

			puts("PUT Successful");

			MessagePUT_REPLY *reply = new MessagePUT_REPLY(container_peer->getHostName(),
					container_peer->getListenPortNumber(), putMsg->getSourceHost(),
					putMsg->getSourcePort(), container_peer->getOverlayID(), putMsg->getDstOid(),
					SUCCESS, putMsg->GetDeviceName());

			reply->setIssueTimeStamp(putMsg->getIssueTimeStamp());
			reply->setResolutionHops(putMsg->getOverlayHops());
			reply->setQueueDelay(putMsg->getQueueDelay());
			reply->setProcessingDelay(putMsg->getProcessingDelay());
			reply->setPingLatency(putMsg->getPingLatency());

			plexus->addToOutgoingQueue(reply);
		} //GET
		else if (message->getMessageType() == MSG_PLEXUS_GET)
		{
			container_peer->incrementGet_processed();
            MessageGET *msg = ((MessageGET*) message);
			HostAddress hostAddress;
			if (index_table->lookup(msg->GetDeviceName(), &hostAddress))
			{
				msg->setProcessingEndT(clock());
				msg->updateStatistics();

				puts("Got it :)");
				MessageGET_REPLY *reply = new MessageGET_REPLY(container_peer->getHostName(),
						container_peer->getListenPortNumber(), msg->getSourceHost(),
						msg->getSourcePort(), container_peer->getOverlayID(), msg->getDstOid(),
						SUCCESS, hostAddress, msg->GetDeviceName());

				reply->setIssueTimeStamp(msg->getIssueTimeStamp());
				reply->setResolutionHops(msg->getOverlayHops());
				reply->setQueueDelay(msg->getQueueDelay());
				reply->setProcessingDelay(msg->getProcessingDelay());
				reply->setPingLatency(msg->getPingLatency());

				plexus->addToOutgoingQueue(reply);
				//send message
			}
			else
			{
				msg->setProcessingEndT(clock());
				msg->updateStatistics();

				puts("GET Failed");
				MessageGET_REPLY *reply = new MessageGET_REPLY(container_peer->getHostName(),
						container_peer->getListenPortNumber(), msg->getSourceHost(),
						msg->getSourcePort(), container_peer->getOverlayID(), msg->getDstOid(),
						ERROR_GET_FAILED, hostAddress, msg->GetDeviceName());

				reply->setIssueTimeStamp(msg->getIssueTimeStamp());
				reply->setResolutionHops(msg->getOverlayHops());
				reply->setQueueDelay(msg->getQueueDelay());
				reply->setProcessingDelay(msg->getProcessingDelay());
				reply->setPingLatency(msg->getPingLatency());

				plexus->addToOutgoingQueue(reply);
			}
		} //GET_REPLY
		else if (message->getMessageType() == MSG_PLEXUS_GET_REPLY)
		{
			MessageGET_REPLY *msg = ((MessageGET_REPLY*) message);
			OverlayID srcID(msg->getSrcOid().GetOverlay_id(), msg->getSrcOid().GetPrefix_length());

			HostAddress ha(msg->getSourceHost(), msg->getSourcePort());
			cache->add(srcID, ha);
			Log* g_log = plexus->getGetLog();

			char i_str[300];
			sprintf(i_str, "%s_%s_%d", msg->getSourceHost().c_str(), msg->getDestHost().c_str(),
					msg->getSequenceNo());
			string key = i_str;

			clock_t start_t = msg->getIssueTimeStamp();
			clock_t end_t = clock();

			double total_t = (double) (end_t - start_t) / (double) CLOCKS_PER_SEC;

			double queue_delay = msg->getQueueDelay();
			double processing_delay = msg->getProcessingDelay();
			double ping_delay = msg->getPingLatency();

			double non_network_latency_t = queue_delay + processing_delay + ping_delay;
			double latency = total_t - non_network_latency_t;

			string status = "S";
			if (msg->getStatus() == ERROR_GET_FAILED)
				status = "F";

			LogEntry *entry = new LogEntry(LOG_GET, key.c_str(), "idddddssi",
					msg->getResolutionHops(), latency, queue_delay, processing_delay, ping_delay,
					total_t, status.c_str(), msg->getDeviceName().c_str(),
					msg->getSrcOid().GetOverlay_id());
			//printf("[Processing Thread:]\tNew log entry created: %s %s\n", entry->getKeyString().c_str(), entry->getValueString().c_str());
			plexus->addToLogQueue(entry);
			//cache->print();
		}
		else if (message->getMessageType() == MSG_PLEXUS_PUT_REPLY)
		{
			MessagePUT_REPLY *msg = (MessagePUT_REPLY*) message;
			OverlayID srcID(msg->getSrcOid().GetOverlay_id(), msg->getSrcOid().GetPrefix_length());

			HostAddress ha(msg->getSourceHost(), msg->getSourcePort());
			cache->add(srcID, ha);
			Log* p_log = plexus->getPutLog();

			char i_str[300];
			sprintf(i_str, "%s_%s_%d", msg->getSourceHost().c_str(), msg->getDestHost().c_str(),
					msg->getSequenceNo());

			string key = i_str;

			clock_t start_t = msg->getIssueTimeStamp();
			clock_t end_t = clock();

			double total_t = (double) (end_t - start_t) / (double) CLOCKS_PER_SEC;

			double queue_delay = msg->getQueueDelay();
			double processing_delay = msg->getProcessingDelay();
			double ping_delay = msg->getPingLatency();

			double non_network_latency_t = queue_delay + processing_delay + ping_delay;
			double latency = total_t - non_network_latency_t;

			LogEntry *entry = new LogEntry(LOG_PUT, key.c_str(), "idddddsi",
					msg->getResolutionHops(), latency, queue_delay, processing_delay, ping_delay,
					total_t, msg->getDeviceName().c_str(),
					msg->getSrcOid().GetOverlay_id());
			//printf("[Processing Thread:]\tNew log entry created: %s %s\n", entry->getKeyString().c_str(), entry->getValueString().c_str());
			plexus->addToLogQueue(entry);
			//cache->print();
		}
		//INIT Message
		else if (message->getMessageType() == MSG_PEER_INIT)
		{
			PeerInitMessage* pInitMsg = (PeerInitMessage*) message;

			container_protocol->setRoutingTable(&pInitMsg->getRoutingTable());
			container_protocol->setIndexTable(new LookupTable<string, HostAddress>());

			container_peer->setNPeers(pInitMsg->getNPeers());
			GlobalData::network_size = pInitMsg->getNPeers();
			container_peer->setOverlayID(pInitMsg->getDstOid());

			container_peer->SetInitRcvd(true);
			container_peer->setPublish_name_range_start(pInitMsg->getPublish_name_range_start());
			container_peer->setPublish_name_range_end(pInitMsg->getPublish_name_range_end());
			container_peer->setLookup_name_range_start(pInitMsg->getLookup_name_range_start());
			container_peer->setLookup_name_range_end(pInitMsg->getLookup_name_range_end());
                        container_peer->SetWebserverPort(pInitMsg->getWebserverPort());

			this->setup(container_protocol->getRoutingTable(), container_protocol->getIndexTable());
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
			printf("Processing peer init get msg, oid = %d\n",
					pInitGet->getDstOid().GetOverlay_id());
			container_protocol->get(pInitGet->getDeviceName());
		}
		else if (message->getMessageType() == MSG_PEER_INITIATE_PUT)
		{
			PeerInitiatePUT* pInitPut = (PeerInitiatePUT*) message;
			container_protocol->put(pInitPut->getDeviceName(), pInitPut->GetHostAddress());
		}
		else if (message->getMessageType() == MSG_PEER_START)
		{
			container_peer->setStatus(PEER_RUNNING);
		}
		else if (message->getMessageType() == MSG_PEER_CHANGE_STATUS)
		{
			PeerChangeStatusMessage* changeStatusMSG = (PeerChangeStatusMessage*) message;
			container_protocol->getContainerPeer()->setStatus(changeStatusMSG->getPeer_status());
		}
		else if (message->getMessageType() == MSG_GENERATE_NAME)
		{
			container_peer->SetStart_gen_name(true);
		}
		else if (message->getMessageType() == MSG_DYN_CHANGE_STATUS)
		{
			PeerDynChangeStatusMessage* dcsMsg = (PeerDynChangeStatusMessage*) message;
			container_peer->SetDyn_status(dcsMsg->getDynStatus());
		}

		return false;
	}

};

#endif	/* PLEXUS_MESSAGE_PROCESSOR_H */

