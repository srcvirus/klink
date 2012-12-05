/*
 * protocol.h
 *
 *  Created on: 2012-11-21
 *      Author: sr2chowd
 */

#ifndef PROTOCOL_H_
#define PROTOCOL_H_

#include "../message/message.h"
#include "../message/message_processor.h"
#include "../ds/lookup_table.h"
#include "../ds/cache.h"

class ABSProtocol {
protected:
    LookupTable<OverlayID, string>* routing_table;
    LookupTable* index_table;
    Cache* cache;
    MessageProcessor* msgProcessor;
public:

    ABSProtocol(LookupTable* routing_table, LookupTable* index_table, 
            Cache *cache, MessageProcessor* msgProcessor) {
        this->routing_table = routing_table;
        this->index_table = index_table;
        this->cache = cache;
        msgProcessor->setup(routing_table, index_table);
    }
    virtual ~ABSProtocol();

    virtual void initiate_join() = 0;
    virtual void process_join() = 0;
    virtual void forward(const ABSMessage*) = 0;
    virtual void get() = 0;
    virtual void put() = 0;
    virtual void rejoin() = 0;
};


#endif /* PROTOCOL_H_ */
