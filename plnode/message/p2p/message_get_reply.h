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
	int resolution_status;
	HostAddress host_address;

public:

	MessageGET_REPLY(){}

    MessageGET_REPLY(string source_host, int source_port, string dest_host, int dest_port
			, OverlayID src_oid, OverlayID dst_id, int status, HostAddress h_address) : ABSMessage(MSG_PLEXUS_GET_REPLY, source_host, source_port, dest_host,
					dest_port, src_oid, dst_id)
    {
        resolution_status = status;
        host_address = h_address;
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

    void setHostAddress(HostAddress& h_address)
    {
    	host_address = h_address;
    }

    HostAddress getHostAddress() const
    {
    	return host_address;
    }

    int getStatus()
    {
    	return resolution_status;
    }

    void setStatus(int status)
    {
    	resolution_status = status;
    }
};


#endif	/* MESSAGE_GET_REPLY_H */

