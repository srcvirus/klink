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

    MessageGET_REPLY()
    {
    	;
    }

    MessageGET_REPLY(string source_host, int source_port, string dest_host, int dest_port
            , OverlayID src_oid, OverlayID dst_id, int status, HostAddress h_address) : ABSMessage(MSG_PLEXUS_GET_REPLY, source_host, source_port, dest_host,
    dest_port, src_oid, dst_id) {
        resolution_status = status;
        host_address = h_address;
    }

    size_t getSize()
    {
    	int ret = getBaseSize();
    	ret += sizeof(int) * 3 + sizeof(char) * host_address.GetHostName().size();
    	return ret;
    }

    char* serialize(int* serialize_length)
    {
    	int parent_length = 0;
    	char* parent_buffer = ABSMessage::serialize(&parent_length);
    	*serialize_length = getSize();

        char* buffer = new char[*serialize_length];
        int offset = 0;

        memcpy(buffer + offset, parent_buffer, parent_length); offset += parent_length;
        memcpy(buffer + offset, (char*)&resolution_status, sizeof(int)); offset += sizeof(int);

        int destHostLength = host_address.GetHostName().size();
        memcpy(buffer + offset, (char*)&destHostLength, sizeof(int)); offset += sizeof(int);
        for(int i = 0; i < destHostLength; i++)
        {
        	char ch = host_address.GetHostName()[i];
        	memcpy(buffer + offset, (char*)&ch, sizeof(char)); offset += sizeof(char);
        }
        int port = host_address.GetHostPort();
        memcpy(buffer + offset, (char*)&port, sizeof(int)); offset += sizeof(int);

        delete[] parent_buffer;
        return buffer;

    }

    ABSMessage* deserialize(char* buffer, int buffer_length)
    {
    	ABSMessage::deserialize(buffer, buffer_length);
    	int offset = getBaseSize();

    	memcpy(&resolution_status, buffer + offset, sizeof(int)); offset += sizeof(int);
    	int hostLength = 0;
    	memcpy(&hostLength, buffer + offset, sizeof(int)); offset += sizeof(int);

    	string host = "";
    	for(int i = 0; i < hostLength; i++)
    	{
    		char ch;
    		memcpy(&ch, buffer + offset, sizeof(int)); offset += sizeof(char);
    		host += ch;
    	}

    	host_address.SetHostName(host);
    	int port = 0;
    	memcpy(&port, buffer + offset, sizeof(int)); offset += sizeof(int);
    	host_address.SetHostPort(port);

        return this;
    }

    void setHostAddress(HostAddress& h_address) {
        host_address = h_address;
    }

    HostAddress getHostAddress() const {
        return host_address;
    }

    int getStatus() {
        return resolution_status;
    }

    void setStatus(int status) {
        resolution_status = status;
    }
};


#endif	/* MESSAGE_GET_REPLY_H */

