/* 
 * File:   plexus_message_processor.h
 * Author: mfbari
 *
 * Created on November 29, 2012, 2:45 PM
 */

#ifndef PLEXUS_MESSAGE_PROCESSOR_H
#define	PLEXUS_MESSAGE_PROCESSOR_H

#include "../../message/message.h"
#include "../../message/message_processor.h"

class PlexusMessageProcessor : public MessageProcessor {
public:

    void setup(LookupTable* routing_table, LookupTable* index_table){
        MessageProcessor::setup(routing_table, index_table);
    }
    
    bool processMessage(ABSMessage* message) {

        //PUT
        if (message->getMessageType() == MSG_PLEXUS_PUT) {
            index_table->add(((MessagePUT*)message)->GetDeviceName(), 
                    ((MessagePUT*)message)->GetIp());
        }
        //GET
        else if (message->getMessageType() == MSG_PLEXUS_GET) {
            
        }
        //GET REPLY
        else if (message->getMessageType() == MSG_PLEXUS_GET_REPLY) {
            MessageGET_REPLY *msg = ((MessageGET_REPLY*)message);
            cache->add(msg->getID(), msg->getIP());
        }
    }
};

#endif	/* PLEXUS_MESSAGE_PROCESSOR_H */

