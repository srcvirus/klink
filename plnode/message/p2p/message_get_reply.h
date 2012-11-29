/* 
 * File:   message_get_reply.h
 * Author: mfbari
 *
 * Created on November 29, 2012, 4:31 PM
 */

#ifndef MESSAGE_GET_REPLY_H
#define	MESSAGE_GET_REPLY_H

#include "../message.h"
#include "../../ds/ip_address.h"

#include <cstring>

class MessageGET_REPLY : public ABSMessage {
    OverlayID *id;
    IPAddress *ip;
public:

    void setID(OverlayID *id){
        this->id = id;
    }
    
    OverlayID* getID() const {
        return id;
    }
    
    void setIP(IPAddress *ip) {
        this->ip = *ip;
    }

    IPAddress* getIP() const {
        return ip;
    }
};


#endif	/* MESSAGE_GET_REPLY_H */

