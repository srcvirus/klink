/* 
 * File:   plexus_protocol.h
 * Author: mfbari
 *
 * Created on November 29, 2012, 3:10 PM
 */

#ifndef PLEXUS_PROTOCOL_H
#define	PLEXUS_PROTOCOL_H

#include "../protocol.h"
#include "../../ds/cache.h"
#include "../../message/message_processor.h"
#include "../../message/p2p/message_get.h"
#include "../../message/p2p/message_put.h"
#include "../../message/p2p/message_get_reply.h"

class PlexusProtocol : public ABSProtocol {
public:

    PlexusProtocol() {
    }

    PlexusProtocol(LookupTable* routing_table, LookupTable* index_table,
            Cache cache, MessageProcessor* msgProcessor) :
    ABSProtocol(routing_table, index_table, cache, msgProcessor) {

    }

    bool processMessage(ABSMessage *message) {
        return msgProcessor->processMessage(message);
    }

    void initiate_join() {
    }

    void process_join() {
    }

    void forward(const ABSMessage* msg) {
        int maxLengthMatch = 0, currentMatchLength;
        OverlayID idWithLongestMatch;
        //search in the RT
        routing_table->reset_iterator();
        while (routing_table->hasMoreKey()) {
            OverlayID oid = routing_table->getNextKey();
            currentMatchLength = msg->getOID().GetMatchedPrefixLength(oid);
            if (currentMatchLength > maxLengthMatch) {
                maxLengthMatch = currentMatchLength;
                idWithLongestMatch = msg->getOID();
            }
        }
        //search in the CAche
        cache->reset_iterator();
        while (cache->has_next()) {
            DLLNode *node = cache->get_next();
            OverlayID *id = node->key;
            currentMatchLength = msg->getOID().GetMatchedPrefixLength(*id);
            if (currentMatchLength > maxLengthMatch) {
                maxLengthMatch = currentMatchLength;
                idWithLongestMatch = msg->getOID();
            }
        }

        //push in Q with idWithLongestMatchPlexusProtocol
    }

    void get(string name) {
        MessageGET *msg = new MessageGET();
        msg->SetDeviceName(name);
        //push in Q
    }

    void put(string name, HostAddress hostAddress) {
        MessagePUT *msg = new MessagePUT();
        msg->SetDeviceName(name);
        msg->SetHostAddress(hostAddress);
        //push message in Q
    }

    void rejoin() {
    }
};

#endif	/* PLEXUS_PROTOCOL_H */

