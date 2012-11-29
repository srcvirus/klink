/* 
 * File:   message_processor.h
 * Author: mfbari
 *
 * Created on November 29, 2012, 2:45 PM
 */

#ifndef MESSAGE_PROCESSOR_H
#define	MESSAGE_PROCESSOR_H

#include "../ds/cache.h"
#include "../ds/lookup_table.h"

class MessageProcessor {
protected:
    LookupTable* routing_table;
    LookupTable* index_table;
    Cache* cache;
public:
    void setup(LookupTable* routing_table, LookupTable* index_table, Cache *cache){
        this->routing_table = routing_table;
        this->index_table = index_table;
        this->cache = cache;
    }
    
    virtual bool processMessage(ABSMessage* message) = 0;
};

#endif	/* MESSAGE_PROCESSOR_H */

