/*
 * message.h
 *
 *  Created on: 2012-11-21
 *      Author: sr2chowd
 */

#ifndef MESSAGE_H_
#define MESSAGE_H_

#include <string>
#include <stdio.h>
#include "../ds/overlay_id.h"
#include "../ds/GlobalData.h"

using namespace std;

//Message Types
#define MSG_PEER_CONFIG					1
#define MSG_PEER_START					2
#define MSG_PEER_CHANGE_STATUS			3
#define MSG_PEER_INIT					4
#define MSG_PEER_INITIATE_GET			5
#define MSG_PEER_INITIATE_PUT			6
#define MSG_DYN_CHANGE_STATUS			11
#define MSG_DYN_CONFIG					12
#define MSG_LOOKUP_CHANGE_STATUS		21
#define MSG_GENERATE_NAME				31
#define	MSG_PLEXUS_PUT					41
#define MSG_PLEXUS_GET					42
#define MSG_PLEXUS_GET_REPLY			43
#define MSG_PLEXUS_PUT_REPLY			44

class ABSMessage
{
	static int sequence_no_generator;

protected:
	unsigned char messageType;
	unsigned int sequence_no;

	string dest_host;
	int dest_port;
	string source_host;
	int source_port;

	unsigned char overlay_hops;
	unsigned char overlay_ttl;

	OverlayID dst_oid;
	OverlayID src_oid;

	clock_t issue_time_stamp;

	clock_t in_queue_push_time_stamp, in_queue_pop_time_stamp;
	clock_t out_queue_push_time_stamp, out_queue_pop_time_stamp;
	clock_t processing_start_t, processing_end_t;
	clock_t ping_start_t, ping_end_t;

	double queue_delay;
	double processing_delay;
	double ping_latency;

	size_t getBaseSize()
	{
		size_t size = sizeof(char) * 3
				+ sizeof(int) * 11
				+ sizeof(char) * (dest_host.size() + source_host.size())
				+ sizeof(double) * 3
                + sizeof(clock_t);

		return size;
	}

public:

	ABSMessage()
	{
		sequence_no = sequence_no_generator++;
		overlay_hops = 0;
		dest_host = "";
		source_host = "";
		calculateOverlayTTL(GlobalData::network_size);
		ping_latency = queue_delay = processing_delay = 0;
		in_queue_pop_time_stamp = in_queue_push_time_stamp = 0;
		out_queue_pop_time_stamp = out_queue_push_time_stamp = 0;
		processing_start_t = processing_end_t = 0;
		ping_start_t = ping_end_t = 0;
	}

	ABSMessage(unsigned char messageType, string source_host, int source_port, string dest_host,
			int dest_port, OverlayID src_oid, OverlayID dst_oid) :
			messageType(messageType), source_host(source_host), source_port(source_port), dest_host(
					dest_host), dest_port(dest_port), src_oid(src_oid), dst_oid(dst_oid)
	{
		sequence_no = sequence_no_generator++;
		overlay_hops = 0;
		calculateOverlayTTL(GlobalData::network_size);

		ping_latency = queue_delay = processing_delay = 0;
		in_queue_pop_time_stamp = in_queue_push_time_stamp = 0;
		out_queue_pop_time_stamp = out_queue_push_time_stamp = 0;
		processing_start_t = processing_end_t = 0;
		ping_start_t = ping_end_t = 0;
	}

	virtual size_t getSize()
	{
		return getBaseSize();
	}

	virtual ~ABSMessage()
	{
		;
	}

	void updateStatistics()
	{
		double q_delay = (double)(in_queue_pop_time_stamp - in_queue_push_time_stamp);
		q_delay += (double) (out_queue_pop_time_stamp - out_queue_push_time_stamp);
		q_delay /= (double)CLOCKS_PER_SEC;

		this->queue_delay += q_delay;
		this->processing_delay += ((double)(processing_end_t - processing_end_t) / (double)CLOCKS_PER_SEC);
		this->ping_latency += ((double)(ping_end_t - ping_start_t) / (double)CLOCKS_PER_SEC);
	}

	virtual char* serialize(int* serialize_length)
	{
		char* buffer;
		*serialize_length = getBaseSize();
		buffer = new char[*serialize_length];

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

		memcpy(buffer + offset, (char*) (&issue_time_stamp), sizeof(clock_t));
		offset += sizeof(clock_t);

		memcpy(buffer + offset, (char*)&queue_delay, sizeof(double)); offset += sizeof(double);
		memcpy(buffer + offset, (char*)&processing_delay, sizeof(double)); offset += sizeof(double);
		memcpy(buffer + offset, (char*)&ping_latency, sizeof(double)); offset += sizeof(double);

		int o_id, p_len, m_len;
		o_id = dst_oid.GetOverlay_id();
		p_len = dst_oid.GetPrefix_length();
		m_len = dst_oid.MAX_LENGTH;

		memcpy(buffer + offset, (char*) &o_id, sizeof(int));
		offset += sizeof(int);
		memcpy(buffer + offset, (char*) &p_len, sizeof(int));
		offset += sizeof(int);
		memcpy(buffer + offset, (char*) &m_len, sizeof(int));
		offset += sizeof(int);

		o_id = src_oid.GetOverlay_id();
		p_len = src_oid.GetPrefix_length();
		m_len = src_oid.MAX_LENGTH;

		memcpy(buffer + offset, (char*) &o_id, sizeof(int));
		offset += sizeof(int);
		memcpy(buffer + offset, (char*) &p_len, sizeof(int));
		offset += sizeof(int);
		memcpy(buffer + offset, (char*) &m_len, sizeof(int));
		offset += sizeof(int);

		return buffer;
	}

	virtual ABSMessage* deserialize(char* buffer, int buffer_len)
	{
		int offset = 0;
		memcpy(&messageType, buffer + offset, sizeof(char));
		offset += sizeof(char); //printf("offset = %d\n", offset);
		memcpy(&sequence_no, buffer + offset, sizeof(int));
		offset += sizeof(int); //printf("offset = %d\n", offset);

		int destHostLength = 0;
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

		int sourceHostLength = 0;
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

		memcpy(&issue_time_stamp, buffer + offset, sizeof(clock_t));
		offset += sizeof(clock_t);

		memcpy(&queue_delay, buffer + offset, sizeof(double)); offset += sizeof(double);
		memcpy(&processing_delay, buffer + offset, sizeof(double)); offset += sizeof(double);
		memcpy(&ping_latency, buffer + offset, sizeof(double)); offset += sizeof(double);

		int o_id, p_len, m_len;

		memcpy(&o_id, buffer + offset, sizeof(int));
		offset += sizeof(int);
		memcpy(&p_len, buffer + offset, sizeof(int));
		offset += sizeof(int);
		memcpy(&m_len, buffer + offset, sizeof(int));
		offset += sizeof(int);

		dst_oid.SetOverlay_id(o_id);
		dst_oid.SetPrefix_length(p_len);
		dst_oid.MAX_LENGTH = m_len;

		memcpy(&o_id, buffer + offset, sizeof(int));
		offset += sizeof(int);
		memcpy(&p_len, buffer + offset, sizeof(int));
		offset += sizeof(int);
		memcpy(&m_len, buffer + offset, sizeof(int));
		offset += sizeof(int);

		src_oid.SetOverlay_id(o_id);
		src_oid.SetPrefix_length(p_len);
		src_oid.MAX_LENGTH = m_len;

		return this;
	}

	virtual void message_print_dump()
	{
		printf("Sequence no. %d\n", sequence_no);
		printf("Message Type. %d\n", messageType);
		printf("Destination host %s\n", dest_host.c_str());
		printf("Destination Port %d\n", dest_port);
		printf("Source Host %s\n", source_host.c_str());
		printf("Source Port %d\n", source_port);
		printf("Source Overlay ID: %d\n", src_oid.GetOverlay_id());
		printf("Destination Overlay ID: %d\n", dst_oid.GetOverlay_id());
		printf("Overlay Hops %d\n", overlay_hops);
		printf("Overlay TTL %d\n", overlay_ttl);
		printf("Issue time stamp = %ld\n", issue_time_stamp);
	}

	clock_t getIssueTimeStamp() const
	{
		return issue_time_stamp;
	}

	void setIssueTimeStamp()
	{
		issue_time_stamp = clock();
	}

	void setIssueTimeStamp(clock_t timestamp)
	{
		issue_time_stamp = timestamp;
	}

	string getDestHost()
	{
		return dest_host;
	}

	/*void setDestHost(const string& destHost)
	 {
	 dest_host = destHost;
	 }*/

	void setDestHost(const char* destHost)
	{
		dest_host = destHost;
	}

	int getDestPort()
	{
		return dest_port;
	}

	void setDestPort(const int destPort)
	{
		dest_port = destPort;
	}

	unsigned char getOverlayHops()
	{
		return overlay_hops;
	}

	void setOverlayHops(unsigned char overlayHops)
	{
		overlay_hops = overlayHops;
	}

	unsigned char getOverlayTtl()
	{
		return overlay_ttl;
	}

	void setOverlayTtl(unsigned char overlayTtl)
	{
		overlay_ttl = overlayTtl;
	}

	void decrementOverlayTtl()
	{
		overlay_ttl--;
	}

	void incrementOverlayHops()
	{
		overlay_hops++;
	}

	string getSourceHost()
	{
		return source_host;
	}

	void setSourceHost(const string& sourceHost)
	{
		source_host = sourceHost;
	}

	void setSourceHost(const char* sourceHost)
	{
		source_host = sourceHost;
	}

	int getSourcePort()
	{
		return source_port;
	}

	void setSourcePort(int sourcePort)
	{
		source_port = sourcePort;
	}

	int getSequenceNo()
	{
		return sequence_no;
	}

	void setMessageType(unsigned char messageType)
	{
		this->messageType = messageType;
	}

	unsigned char getMessageType() const
	{
		return messageType;
	}

	void setSrcOid(OverlayID oID)
	{
		this->src_oid = oID;
	}

	OverlayID getSrcOid() const
	{
		return src_oid;
	}

	void setDstOid(OverlayID oID)
	{
		this->dst_oid = oID;
	}

	OverlayID getDstOid() const
	{
		return dst_oid;
	}

	void calculateOverlayTTL(int networkSize)
	{
		overlay_ttl = 10;
		//overlay_ttl = ceil(log2(n)) + 2;//(int) floor(log10(n) / log(2.0)) + 2;
	}

	clock_t getInQueuePopTimeStamp() const
	{
		return in_queue_pop_time_stamp;
	}

	void setInQueuePopTimeStamp(clock_t inQueuePopTimeStamp)
	{
		in_queue_pop_time_stamp = inQueuePopTimeStamp;
	}

	clock_t getInQueuePushTimeStamp() const
	{
		return in_queue_push_time_stamp;
	}

	void setInQueuePushTimeStamp(clock_t inQueuePushTimeStamp)
	{
		in_queue_push_time_stamp = inQueuePushTimeStamp;
	}

	clock_t getOutQueuePopTimeStamp() const
	{
		return out_queue_pop_time_stamp;
	}

	void setOutQueuePopTimeStamp(clock_t outQueuePopTimeStamp)
	{
		out_queue_pop_time_stamp = outQueuePopTimeStamp;
	}

	clock_t getOutQueuePushTimeStamp() const
	{
		return out_queue_push_time_stamp;
	}

	void setOutQueuePushTimeStamp(clock_t outQueuePushTimeStamp)
	{
		out_queue_push_time_stamp = outQueuePushTimeStamp;
	}

	double getPingLatency() const
	{
		return ping_latency;
	}

	void setPingLatency(double pingLatency)
	{
		ping_latency = pingLatency;
	}

	clock_t getPingEndT() const
	{
		return ping_end_t;
	}

	void setPingEndT(clock_t pingEndT)
	{
		ping_end_t = pingEndT;
	}

	clock_t getPingStartT() const
	{
		return ping_start_t;
	}

	void setPingStartT(clock_t pingStartT)
	{
		ping_start_t = pingStartT;
	}

	double getProcessingDelay() const
	{
		return processing_delay;
	}

	void setProcessingDelay(double processingDelay)
	{
		processing_delay = processingDelay;
	}

	double getQueueDelay() const
	{
		return queue_delay;
	}

	void setQueueDelay(double queueDelay)
	{
		queue_delay = queueDelay;
	}

	clock_t getProcessingEndT() const
	{
		return processing_end_t;
	}

	void setProcessingEndT(clock_t processingEndT)
	{
		processing_end_t = processingEndT;
	}

	clock_t getProcessingStartT() const
	{
		return processing_start_t;
	}

	void setProcessingStartT(clock_t processingStartT)
	{
		processing_start_t = processingStartT;
	}
};

int ABSMessage::sequence_no_generator;

#endif /* MESSAGE_H */
