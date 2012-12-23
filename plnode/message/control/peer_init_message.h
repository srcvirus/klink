/*
 * peer_init_message.h
 *
 *  Created on: 2012-12-06
 *      Author: sr2chowd
 */

#ifndef PEER_INIT_MESSAGE_H_
#define PEER_INIT_MESSAGE_H_

#include "../../../plnode/ds/host_address.h"
#include "../../../plnode/ds/overlay_id.h"
#include "../../../plnode/ds/lookup_table.h"
#include "../../../plnode/ds/lookup_table_iterator.h"
#include "../message.h"
#include <memory.h>

/*
 * unsigned char messageType;
 unsigned int sequence_no;
 string dest_host;
 int dest_port;
 string source_host;
 int source_port;
 unsigned char overlay_hops;
 unsigned char overlay_ttl;
 OverlayID oID;
 */
class PeerInitMessage: public ABSMessage
{
	LookupTable<OverlayID, HostAddress> routing_table;
	int n_peers, k;
	double alpha;
	int publish_name_range_start, publish_name_range_end;
	int lookup_name_range_start, lookup_name_range_end;
        int webserver_port;

public:
	PeerInitMessage()
	{
		messageType = MSG_PEER_INIT;
	}

	void setRoutingTable(LookupTable<OverlayID, HostAddress>& r_table)
	{
		routing_table = r_table;
	}

	LookupTable<OverlayID, HostAddress>& getRoutingTable()
	{
		return routing_table;
	}

	void setNPeers(int n)
	{
		n_peers = n;
	}
	int getNPeers()
	{
		return n_peers;
	}

	void setK(int k)
	{
		this->k = k;
	}
	int getK()
	{
		return k;
	}

	void setAlpha(double alpha)
	{
		this->alpha = alpha;
	}
	double getAlpha()
	{
		return alpha;
	}

	size_t getSize()
	{
		size_t ret = getBaseSize();
		ret += sizeof(int) * 4;
		ret += sizeof(double);
		ret += sizeof(int) * 4;
		LookupTableIterator<OverlayID, HostAddress> r_iterator(
				&routing_table);
		r_iterator.reset_iterator();

		while (r_iterator.hasMoreKey())
		{
			OverlayID key = r_iterator.getNextKey();
			HostAddress value;
			routing_table.lookup(key, &value);
			ret += sizeof(int) * 5;
			ret += sizeof(char) * value.GetHostName().size();
		}
		return ret;
	}

	virtual char* serialize(int* serialize_length)
	{
		*serialize_length = getSize();
		LookupTableIterator<OverlayID, HostAddress> rtable_iterator(
				&routing_table);

		int offset = 0;
		int parent_size = 0;

		char* parent_buffer = ABSMessage::serialize(&parent_size);
		char* buffer = new char[*serialize_length];
		memcpy(buffer + offset, parent_buffer, parent_size);
		offset += parent_size;
		//delete[] parent_buffer;

		memcpy(buffer + offset, (char*)(&n_peers), sizeof(int));
		offset += sizeof(int);
		memcpy(buffer + offset, (char*)(&k), sizeof(int));
		offset += sizeof(int);
		memcpy(buffer + offset, (char*)(&alpha), sizeof(double));
		offset += sizeof(double);

		memcpy(buffer + offset, (char*)(&publish_name_range_start),sizeof(int));
		offset += sizeof(int);
		memcpy(buffer + offset, (char*)(&publish_name_range_end), sizeof(int));
		offset += sizeof(int);
		memcpy(buffer + offset, (char*)(&lookup_name_range_start), sizeof(int));
		offset += sizeof(int);
		memcpy(buffer + offset, (char*)(&lookup_name_range_end), sizeof(int));
		offset += sizeof(int);
		memcpy(buffer + offset, (char*)(&webserver_port), sizeof(int));
		offset += sizeof(int);

		int routingTableSize = routing_table.size();
		memcpy(buffer + offset, (char*)(&routingTableSize), sizeof(int));
		offset += sizeof(int);

		rtable_iterator.reset_iterator();
		OverlayID key;
		HostAddress value;
		while(rtable_iterator.hasMoreKey())
		{
			key = rtable_iterator.getNextKey();

			routing_table.lookup(key, &value);
			int hostNameLength = value.GetHostName().size();

			int o_id = key.GetOverlay_id(), p_len = key.GetPrefix_length(), m_len = key.MAX_LENGTH;

			memcpy(buffer + offset, (char*)&o_id, sizeof(int)); offset += sizeof(int);
			memcpy(buffer + offset, (char*)&p_len, sizeof(int)); offset += sizeof(int);
			memcpy(buffer + offset, (char*)&m_len, sizeof(int)); offset += sizeof(int);

			memcpy(buffer + offset, (char*) (&hostNameLength), sizeof(int));
			offset += sizeof(int);
			const char* str = value.GetHostName().c_str();

			for (int i = 0; i < hostNameLength; i++)
			{
				memcpy(buffer + offset, (char*)(str + i), sizeof(char));
				offset += sizeof(char);
			}

			int hostport = value.GetHostPort();
			memcpy(buffer + offset, (char*) (&hostport), sizeof(int));
			offset += sizeof(int);
		}
		return buffer;
	}

	virtual ABSMessage* deserialize(char* buffer, int buffer_length)
	{
		int offset = 0;
		routing_table.clear();

		ABSMessage::deserialize(buffer, buffer_length);
		offset = getBaseSize();

		memcpy(&n_peers, buffer + offset, sizeof(int));
		offset += sizeof(int);
		memcpy(&k, buffer + offset, sizeof(int));
		offset += sizeof(int);
		memcpy(&alpha, buffer + offset, sizeof(double));
		offset += sizeof(double);

		memcpy(&publish_name_range_start, buffer + offset, sizeof(int));
		offset += sizeof(int);
		memcpy(&publish_name_range_end, buffer + offset, sizeof(int));
		offset += sizeof(int);
		memcpy(&lookup_name_range_start, buffer + offset, sizeof(int));
		offset += sizeof(int);
		memcpy(&lookup_name_range_end, buffer + offset, sizeof(int));
		offset += sizeof(int);
		memcpy(&webserver_port, buffer + offset, sizeof(int));
		offset += sizeof(int);

		int routingTableSize;
		memcpy(&routingTableSize, buffer + offset, sizeof(int));
		offset += sizeof(int); //printf("offset = %d\n", offset);
		printf("%d\n", routingTableSize);

		for (int i = 0; i < routingTableSize; i++)
		{
			OverlayID key;
			HostAddress value;
			int hostNameLength;
			string hostname;
			int hostport;

			int o_id, p_len, m_len;
			memcpy(&o_id, buffer + offset, sizeof(int)); offset += sizeof(int);
			memcpy(&p_len, buffer + offset, sizeof(int)); offset += sizeof(int);
			memcpy(&m_len, buffer + offset, sizeof(int)); offset += sizeof(int);

			key.SetOverlay_id(o_id);
			key.SetPrefix_length(p_len);
			key.MAX_LENGTH = m_len;

			memcpy(&hostNameLength, buffer + offset, sizeof(int));
			offset += sizeof(int); //printf("offset = %d\n", offset);
			char ch;

			for (int i = 0; i < hostNameLength; i++)
			{
				memcpy(&ch, buffer + offset, sizeof(char));
				hostname += ch;
				offset += sizeof(char); //printf("offset = %d\n", offset);
			}

			memcpy(&hostport, buffer + offset, sizeof(int));
			offset += sizeof(int); //printf("offset = %d\n", offset);
			value.SetHostName(hostname);
			value.SetHostPort(hostport);
			routing_table.add(key, value);
		}

		return this;
	}

	virtual void message_print_dump()
	{
		puts("<<<<<<<<<<<<<<<<<<<<>>>>>>>>>>>>>>>>>>>>");
		printf("Init Message\n");
		ABSMessage::message_print_dump();
		LookupTableIterator<OverlayID, HostAddress> rtable_iterator(
				&routing_table);
		//routing_table.reset_iterator();
		rtable_iterator.reset_iterator();

		while (rtable_iterator.hasMoreKey())
		//while(routing_table.hasMoreKey())
		{
			OverlayID key = rtable_iterator.getNextKey();
			//OverlayID key = routing_table.getNextKey();
			HostAddress value;
			routing_table.lookup(key, &value);
			printf("Overlay ID = %d, Hostname = %s, Host Port = %d\n",
					key.GetOverlay_id(), value.GetHostName().c_str(),
					value.GetHostPort());
		}
		printf("N Peers = %d\n", n_peers);
		printf("Alpha = %.4lf\n", alpha);
		printf("K = %d\n", k);
		printf("Publish Start = %d End = %d\n", publish_name_range_start,
				publish_name_range_end);
		printf("Lookup Start = %d End = %d\n", lookup_name_range_start,
				lookup_name_range_end);
		printf("Webserver Port = %d\n", webserver_port);
		puts("<<<<<<<<<<<<<<<<<<<<>>>>>>>>>>>>>>>>>>>>");
	}

	void setLookup_name_range_end(int lookup_name_range_end)
	{
		this->lookup_name_range_end = lookup_name_range_end;
	}

	int getLookup_name_range_end() const
	{
		return lookup_name_range_end;
	}

	void setLookup_name_range_start(int lookup_name_range_start)
	{
		this->lookup_name_range_start = lookup_name_range_start;
	}

	int getLookup_name_range_start() const
	{
		return lookup_name_range_start;
	}

	void setPublish_name_range_end(int publish_name_range_end)
	{
		this->publish_name_range_end = publish_name_range_end;
	}

	int getPublish_name_range_end() const
	{
		return publish_name_range_end;
	}

	void setPublish_name_range_start(int publish_name_range_start)
	{
		this->publish_name_range_start = publish_name_range_start;
	}

	int getPublish_name_range_start() const
	{
		return publish_name_range_start;
	}

        void setWebserverPort(int webserver_port) {
                this->webserver_port = webserver_port;
        }

        int getWebserverPort() const {
                return webserver_port;
        }

};

#endif /* PEER_INIT_MESSAGE_H_ */
