/* 
 * File:   message_put_reply.h
 * Author: sr2chowd
 *
 * Created on December 21, 2012, 4:31 PM
 */

#ifndef MESSAGE_PUT_REPLY_H
#define	MESSAGE_PUT_REPLY_H

#include "../message.h"
#include "../../ds/overlay_id.h"
#include "../../ds/ip_address.h"

#include <cstring>

class MessagePUT_REPLY: public ABSMessage
{
	int resolution_status;
	int resolution_hops;
	string device_name;

public:

	MessagePUT_REPLY()
	{
		;
	}

	MessagePUT_REPLY(string source_host, int source_port, string dest_host,
			int dest_port, OverlayID src_oid, OverlayID dst_id, int status,
			string device_name) :
			ABSMessage(MSG_PLEXUS_PUT_REPLY, source_host, source_port,
					dest_host, dest_port, src_oid, dst_id)
	{
		resolution_status = status;
		this->device_name = device_name;
	}

	size_t getSize()
	{
		size_t ret = getBaseSize();
		ret += sizeof(int) * 3
			+ sizeof(char) * device_name.size();
		return ret;
	}

	char* serialize(int* serialize_length)
	{
		int parent_length = 0;
		char* parent_buffer = ABSMessage::serialize(&parent_length);
		*serialize_length = getSize();

		char* buffer = new char[*serialize_length];
		int offset = 0;

		memcpy(buffer + offset, parent_buffer, parent_length);
		offset += parent_length;
		memcpy(buffer + offset, (char*) &resolution_status, sizeof(int));
		offset += sizeof(int);
		memcpy(buffer + offset, (char*) &resolution_hops, sizeof(int));
		offset += sizeof(int);

		int deviceNameLength = device_name.size();
		memcpy(buffer + offset, (char*)&deviceNameLength, sizeof(int)); offset += sizeof(int);
		for(int i = 0; i < deviceNameLength; i++)
		{
			char ch = device_name[i];
			memcpy(buffer + offset, (char*)&ch, sizeof(char)); offset += sizeof(char);
		}

		delete[] parent_buffer;
		return buffer;

	}

	ABSMessage* deserialize(char* buffer, int buffer_length)
	{
		ABSMessage::deserialize(buffer, buffer_length);
		int offset = getBaseSize();

		memcpy(&resolution_status, buffer + offset, sizeof(int));
		offset += sizeof(int);
		memcpy(&resolution_hops, buffer + offset, sizeof(int));
		offset += sizeof(int);

		int deviceNameLength = 0;
		memcpy(&deviceNameLength, buffer + offset, sizeof(int)); offset += sizeof(int);

		device_name = "";
		for(int i = 0; i < deviceNameLength; i++)
		{
			char ch;
			memcpy(&ch, buffer + offset, sizeof(char)); offset += sizeof(char);
			device_name += ch;
		}

		return this;
	}

	int getStatus()
	{
		return resolution_status;
	}

	void setStatus(int status)
	{
		resolution_status = status;
	}

	int getResolutionHops() const
	{
		return resolution_hops;
	}

	void setResolutionHops(int hops)
	{
		resolution_hops = hops;
	}

	string getDeviceName()
	{
		return device_name;
	}

	void setDeviceName(string d_name)
	{
		device_name = d_name;
	}
};

#endif	/* MESSAGE_PUT_REPLY_H */

