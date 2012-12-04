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
    
    void initiate_join(){}
    
    void process_join(){}
        
    void forward(const ABSMessage*){
        int maxLengthMatch = 0;        
        OverlayID& idWithLongestMatch = NULL;
        //search in the RT
        
        //search in the CAche
    }
    
    void get(string name){
        MessageGET *msg = new MessageGET();
        msg->SetDeviceName(name);
        //push in Q
    }
    
    
    void put(string name, IPAddress ip){
        MessagePUT *msg = new MessagePUT();
        msg->SetDeviceName(name);
        msg->SetIp(ip);
        //push message in Q
    }
    
    void rejoin() = 0;
};

#endif	/* PLEXUS_PROTOCOL_H */

