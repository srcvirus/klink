/* 
 * File:   message_put.h
 * Author: mfbari
 *
 * Created on November 29, 2012, 3:17 PM
 */

#ifndef MESSAGE_PUT_H
#define	MESSAGE_PUT_H

#include "../message.h"
#include "../../ds/host_address.h"

#include <cstring>

class MessagePUT: public ABSMessage
{
	string deviceName;
	HostAddress hostAddress;
public:
	MessagePUT()
	{
	}

	MessagePUT(string source_host, int source_port, string dest_host,
			int dest_port, OverlayID src_oid, OverlayID dst_id,
			string &deviceName, HostAddress &hostAddress) :
			ABSMessage(MSG_PLEXUS_PUT, source_host, source_port, dest_host,
					dest_port, src_oid, dst_id), deviceName(deviceName), hostAddress(
					hostAddress)
	{
		;
	}

	char* serialize(int* serialize_length)
	{
		*serialize_length = sizeof(char) + sizeof(int) * 8
				+ sizeof(char) * dest_host.size()
				+ sizeof(char) * source_host.size() + 2 * sizeof(char)
				+ sizeof(OverlayID) * 2 + sizeof(char) * deviceName.size()
				+ sizeof(char) * hostAddress.GetHostName().size();

		char* buffer = new char[*serialize_length];
		int offset = 0;
		int destHostLength = dest_host.size();
		int sourceHostLength = source_host.size();

		memcpy(buffer + offset, (char*) (&messageType), sizeof(char));
		offset += sizeof(char);
		memcpy(buffer + offset, (char*) (&sequence_no), sizeof(int));
		offset += sizeof(int);

		memcpy(buffer + offset, (char*) (&destHostLength), sizeof(int));
		offset += sizeof(int);
		for (int i = 0; i < destHostLength; i++)
		{
			char ch = dest_host[i];
			memcpy(buffer + offset, (char*) (&ch), sizeof(char));
			offset += sizeof(char);
		}
		memcpy(buffer + offset, (char*) (&dest_port), sizeof(int));
		offset += sizeof(int);

		memcpy(buffer + offset, (char*) (&sourceHostLength), sizeof(int));
		offset += sizeof(int);
		for (int i = 0; i < sourceHostLength; i++)
		{
			char ch = source_host[i];
			memcpy(buffer + offset, (char*) (&ch), sizeof(char));
			offset += sizeof(char);
		}
		memcpy(buffer + offset, (char*) (&source_port), sizeof(int));
		offset += sizeof(int);

		memcpy(buffer + offset, (char*) (&overlay_hops), sizeof(char));
		offset += sizeof(char);
		memcpy(buffer + offset, (char*) (&overlay_ttl), sizeof(char));
		offset += sizeof(char);

		memcpy(buffer + offset, (char*) (&dst_oid), sizeof(OverlayID));
		offset += sizeof(OverlayID);
		memcpy(buffer + offset, (char*) (&src_oid), sizeof(OverlayID));
		offset += sizeof(OverlayID);

		int deviceNameLength = deviceName.size();
		memcpy(buffer + offset, (char*) (&deviceNameLength), sizeof(int));
		offset += sizeof(int);
		for (int i = 0; i < deviceNameLength; i++)
		{
			char ch = deviceName[i];
			memcpy(buffer + offset, (char*) (&ch), sizeof(char));
			offset += sizeof(char);
		}

		int hostAddrNameLength = hostAddress.GetHostName().size();
		memcpy(buffer + offset, (char*) &hostAddrNameLength, sizeof(int));
		offset += sizeof(int);
		for (int i = 0; i < hostAddrNameLength; i++)
		{
			char ch = hostAddress.GetHostName()[i];
			memcpy(buffer + offset, (char*) (&ch), sizeof(char));
			offset += sizeof(char);
		}
		int hostPort = hostAddress.GetHostPort();
		memcpy(buffer + offset, (char*) &hostPort, sizeof(int));
		offset += sizeof(int);

		return buffer;
	}

	ABSMessage* deserialize(char* buffer, int buffer_length)
	{
		int offset = 0;
		int destHostLength, sourceHostLength, deviceNameLength,
				hostAddrNameLength;

		memcpy(&messageType, buffer + offset, sizeof(char));
		offset += sizeof(char); //printf("offset = %d\n", offset);
		memcpy(&sequence_no, buffer + offset, sizeof(int));
		offset += sizeof(int); //printf("offset = %d\n", offset);
		memcpy(&destHostLength, buffer + offset, sizeof(int));
		offset += sizeof(int); //printf("offset = %d\n", offset);
		dest_host = "";
		//printf("DH Length : %d\n", destHostLength);
		for (int i = 0; i < destHostLength; i++)
		{
			char ch;
			memcpy(&ch, buffer + offset, sizeof(char));
			offset += sizeof(char); //printf("offset = %d\n", offset);
			dest_host += ch;
		}
		memcpy(&dest_port, buffer + offset, sizeof(int));
		offset += sizeof(int); //printf("offset = %d\n", offset);
		memcpy(&sourceHostLength, buffer + offset, sizeof(int));
		offset += sizeof(int); //printf("offset = %d\n", offset);
		source_host = "";
		for (int i = 0; i < sourceHostLength; i++)
		{
			char ch;
			memcpy(&ch, buffer + offset, sizeof(char));
			offset += sizeof(char); //printf("offset = %d\n", offset);
			source_host += ch;
		}
		memcpy(&source_port, buffer + offset, sizeof(int));
		offset += sizeof(int); //printf("offset = %d\n", offset);
		memcpy(&overlay_hops, buffer + offset, sizeof(char));
		offset += sizeof(char); //printf("offset = %d\n", offset);
		memcpy(&overlay_ttl, buffer + offset, sizeof(char));
		offset += sizeof(char); //printf("offset = %d\n", offset);

		memcpy(&dst_oid, buffer + offset, sizeof(OverlayID));
		offset += sizeof(OverlayID); //printf("offset = %d\n", offset);
		memcpy(&src_oid, buffer + offset, sizeof(OverlayID));
		offset += sizeof(OverlayID); //printf("offset = %d\n", offset);

		memcpy(&deviceNameLength, buffer + offset, sizeof(int));
		offset += sizeof(int);
		deviceName = "";
		for (int i = 0; i < deviceNameLength; i++)
		{
			char ch;
			memcpy(&ch, buffer + offset, sizeof(char));
			offset += sizeof(char); //printf("offset = %d\n", offset);
			deviceName += ch;
		}

		memcpy(&hostAddrNameLength, buffer + offset, sizeof(int));
		offset += sizeof(int);
		string hostAddrName = "";
		for (int i = 0; i < hostAddrNameLength; i++)
		{
			char ch;
			memcpy(&ch, buffer + offset, sizeof(char));
			offset += sizeof(char); //printf("offset = %d\n", offset);
			hostAddrName += ch;
		}
		hostAddress.SetHostName(hostAddrName);
		int hPort = 0;
		memcpy(&hPort, buffer + offset, sizeof(int));
		offset += sizeof(int);
		hostAddress.SetHostPort(hPort);

		return this;
	}

	void SetHostAddress(HostAddress &hostAddress)
	{
		this->hostAddress = hostAddress;
	}

	HostAddress GetHostAddress() const
	{
		return hostAddress;
	}

	void SetDeviceName(string &deviceName)
	{
		this->deviceName = deviceName;
	}

	string GetDeviceName() const
	{
		return deviceName;
	}

	virtual void message_print_dump()
	{
		puts(
				"------------------------------<PUT Message>------------------------------");
		ABSMessage::message_print_dump();
		printf("Device Name %s\n", deviceName.c_str());
		printf("Host Address %s:%d\n", hostAddress.GetHostName().c_str(),
				hostAddress.GetHostPort());
		puts(
				"------------------------------</PUT Message>-----------------------------");
	}
};

#endif	/* MESSAGE_PUT_H */

