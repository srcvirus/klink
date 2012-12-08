/* 
 * File:   plexus_protocol.h
 * Author: mfbari
 *
 * Created on November 29, 2012, 3:10 PM
 */

#ifndef PLEXUS_PROTOCOL_H
#define	PLEXUS_PROTOCOL_H

#include <pthread.h>
#include <cstring>
#include <queue>
#include "../protocol.h"
#include "../../ds/cache.h"
#include "../../message/message_processor.h"
#include "plexus_message_processor.h"
#include "../../message/p2p/message_get.h"
#include "../../message/p2p/message_put.h"
#include "../../message/p2p/message_get_reply.h"
#include "../../ds/host_address.h"

using namespace std;

class ABSProtocol;

class PlexusProtocol : public ABSProtocol {
    queue <ABSMessage*> incoming_message_queue;
    queue <ABSMessage*> outgoing_message_queue;

    pthread_mutex_t incoming_queue_lock;
    pthread_mutex_t outgoing_queue_lock;

    pthread_cond_t cond_incoming_queue_empty;
    pthread_cond_t cond_outgoing_queue_empty;

public:

    PlexusProtocol() :
    ABSProtocol() {
        //this->routing_table = new LookupTable<OverlayID, HostAddress > ();
        //this->index_table = new LookupTable<string, OverlayID > ();
        this->msgProcessor = new PlexusMessageProcessor();
        pthread_mutex_init(&incoming_queue_lock, NULL);
        pthread_mutex_init(&outgoing_queue_lock, NULL);

        pthread_cond_init(&cond_incoming_queue_empty, NULL);
        pthread_cond_init(&cond_outgoing_queue_empty, NULL);
    }

    PlexusProtocol(LookupTable<OverlayID, HostAddress>* routing_table,
            LookupTable<string, HostAddress>* index_table,
            Cache *cache,
            MessageProcessor* msgProcessor,
            Peer* container) :
    ABSProtocol(routing_table, index_table, cache, msgProcessor, container) {
        this->msgProcessor->setContainerProtocol(this);

        pthread_mutex_init(&incoming_queue_lock, NULL);
        pthread_mutex_init(&outgoing_queue_lock, NULL);

        pthread_cond_init(&cond_incoming_queue_empty, NULL);
        pthread_cond_init(&cond_outgoing_queue_empty, NULL);
    }

    void processMessage(ABSMessage *message) {
        msgProcessor->processMessage(message);
    }

    void initiate_join() {
    }

    void process_join() {
    }

    bool setNextHop(ABSMessage* msg) {
        int maxLengthMatch = 0, currentMatchLength = 0, currentNodeMathLength = 0;
        HostAddress next_hop;

        msg->decrementOverlayTtl();
        msg->incrementOverlayHops();

        if(msg->getOverlayTtl() == 0)
        	return false;

        Peer *container_peer = getContainerPeer();
        currentNodeMathLength = container_peer->getOverlayID().GetMatchedPrefixLength(msg->getOID());

        //search in the RT
        routing_table->reset_iterator();
        while (routing_table->hasMoreKey()) {
            OverlayID oid = routing_table->getNextKey();
            currentMatchLength = msg->getOID().GetMatchedPrefixLength(oid);
            if (currentMatchLength > maxLengthMatch) {
                maxLengthMatch = currentMatchLength;
                routing_table->lookup(oid, &next_hop);
                printf("next host %s, next port %d\n", next_hop.GetHostName().c_str(), next_hop.GetHostPort());
            }
        }
        //search in the Cache
        cache->reset_iterator();
        while (cache->has_next()) {
            DLLNode *node = cache->get_next();
            OverlayID *id = node->key;
            currentMatchLength = msg->getOID().GetMatchedPrefixLength(*id);
            if (currentMatchLength > maxLengthMatch) {
                maxLengthMatch = currentMatchLength;
                cache->lookup(msg->getOID(), next_hop);
            }
        }

        if (maxLengthMatch == 0 || maxLengthMatch < currentNodeMathLength) {
            return false;
        } else {
            msg->setDestHost(next_hop.GetHostName().c_str());
            msg->setDestPort(next_hop.GetHostPort());
            return true;
        }
    }

    void get(string name) {
        MessageGET *msg = new MessageGET();
        msg->SetDeviceName(name);
        addToOutgoingQueue(msg);
    }

    void put(string name, HostAddress hostAddress) {
        MessagePUT *msg = new MessagePUT();
        msg->SetDeviceName(name);
        msg->SetHostAddress(hostAddress);
        addToOutgoingQueue(msg);
    }

    void rejoin() {
    }

    void addToIncomingQueue(ABSMessage* message) {
        pthread_mutex_lock(&incoming_queue_lock);
        incoming_message_queue.push(message);
        pthread_cond_signal(&cond_incoming_queue_empty);
        pthread_mutex_unlock(&incoming_queue_lock);
    }

    bool isIncomingQueueEmpty() {
        return incoming_message_queue.empty();
    }

    ABSMessage* getIncomingQueueFront() {
        pthread_mutex_lock(&incoming_queue_lock);

        while (incoming_message_queue.empty())
            pthread_cond_wait(&cond_incoming_queue_empty, &incoming_queue_lock);

        ABSMessage* ret = incoming_message_queue.front();
        incoming_message_queue.pop();
        pthread_mutex_unlock(&incoming_queue_lock);
        return ret;
    }

    void addToOutgoingQueue(ABSMessage* message) {
        pthread_mutex_lock(&outgoing_queue_lock);
        outgoing_message_queue.push(message);
        pthread_mutex_unlock(&outgoing_queue_lock);
    }

    bool isOutgoingQueueEmpty() {
        bool status;
        pthread_mutex_lock(&outgoing_queue_lock);
        status = outgoing_message_queue.empty();
        pthread_cond_signal(&cond_outgoing_queue_empty);
        pthread_mutex_unlock(&outgoing_queue_lock);
        return status;
    }

    ABSMessage* getOutgoingQueueFront() {
        pthread_mutex_lock(&outgoing_queue_lock);

        while (outgoing_message_queue.empty())
            pthread_cond_wait(&cond_outgoing_queue_empty, &outgoing_queue_lock);

        ABSMessage* ret = outgoing_message_queue.front();
        outgoing_message_queue.pop();
        pthread_mutex_lock(&outgoing_queue_lock);
        return ret;
    }

    ~PlexusProtocol() {
        pthread_mutex_destroy(&incoming_queue_lock);
        pthread_mutex_destroy(&outgoing_queue_lock);
        pthread_cond_destroy(&cond_incoming_queue_empty);
        pthread_cond_destroy(&cond_outgoing_queue_empty);
    }
};

#endif	/* PLEXUS_PROTOCOL_H */

