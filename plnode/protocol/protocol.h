/*
 * protocol.h
 *
 *  Created on: 2012-11-21
 *      Author: sr2chowd
 */

#ifndef PROTOCOL_H_
#define PROTOCOL_H_

#include <cstring>

#include "../message/message.h"
#include "../message/message_processor.h"
#include "../ds/lookup_table.h"
#include "../ds/cache.h"
#include "../ds/overlay_id.h"
#include "../ds/host_address.h"

class ABSProtocol {
protected:
    LookupTable<OverlayID, HostAddress>* routing_table;
    LookupTable<string, OverlayID>* index_table;
    Cache* cache;
    MessageProcessor* msgProcessor;
public:

    ABSProtocol() {
    }

    ABSProtocol(LookupTable<OverlayID, HostAddress>* routing_table,
            LookupTable<string, OverlayID>* index_table,
            Cache *cache, MessageProcessor* msgProcessor) {
        this->routing_table = routing_table;
        this->index_table = index_table;
        this->cache = cache;
        msgProcessor->setup(routing_table, index_table, cache);
    }
    virtual ~ABSProtocol(){}

    virtual void initiate_join() = 0;
    virtual void process_join() = 0;
    virtual void forward(const ABSMessage*) = 0;
    virtual void get(string name) = 0;
    virtual void put(string name, HostAddress ip) = 0;
    virtual void rejoin() = 0;
};


#endif /* PROTOCOL_H_ */
