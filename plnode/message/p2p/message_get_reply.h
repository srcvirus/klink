/* 
 * File:   message_get_reply.h
 * Author: mfbari
 *
 * Created on November 29, 2012, 4:31 PM
 */

#ifndef MESSAGE_GET_REPLY_H
#define	MESSAGE_GET_REPLY_H

#include "../message.h"
#include "../../ds/overlay_id.h"
#include "../../ds/ip_address.h"

#include <cstring>

class MessageGET_REPLY : public ABSMessage {
    OverlayID *id;
    IPAddress *ip;
public:

    MessageGET_REPLY(OverlayID *id, IPAddress *ip) : ABSMessage() {
        messageType = MSG_PLEXUS_GET_REPLY;
        this->id = id;
        this->ip = ip;
    }

    char* serialize(int* serialize_length) {
        char* buffer = new char[sizeof (MessageGET_REPLY)];
        memcpy(buffer, (char*) (this), sizeof (MessageGET_REPLY));
        *serialize_length = sizeof (MessageGET_REPLY);
        return buffer;
    }

    ABSMessage* deserialize(char* buffer, int buffer_length) {
        memcpy(this, buffer, buffer_length);
        return this;
    }

    void setID(OverlayID *id) {
        this->id = id;
    }

    OverlayID* getID() const {
        return id;
    }

    void setIP(IPAddress *ip) {
        this->ip = ip;
    }

    IPAddress* getIP() const {
        return ip;
    }
};


#endif	/* MESSAGE_GET_REPLY_H */

