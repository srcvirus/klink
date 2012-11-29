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

class PlexusProtocol : public ABSProtocol {
public:

    PlexusProtocol(LookupTable* routing_table, LookupTable* index_table,
            Cache cache, MessageProcessor* msgProcessor) :
    ABSProtocol(routing_table, index_table, cache, msgProcessor) {
           
    }
    
    bool processMessage(ABSMessage *message){
        return msgProcessor->processMessage(message);
    }
};

#endif	/* PLEXUS_PROTOCOL_H */

