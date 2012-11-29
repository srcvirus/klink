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

class PlexusMessageProcessor : MessageProcessor {
public:

    bool processMessage(ABSMessage* message) {

        //PUT
        if (message->getMessageType() == MSG_PLEXUS_PUT) {

        }
        //GET
        else if (message->getMessageType() == MSG_PLEXUS_GET) {

        }
        //GET REPLY
        else if (message->getMessageType() == MSG_PLEXUS_GET_REPLY) {

        }
    }
};

#endif	/* PLEXUS_MESSAGE_PROCESSOR_H */

