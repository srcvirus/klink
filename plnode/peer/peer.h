/*
 * peer.h
 *
 *  Created on: 2012-11-21
 *      Author: sr2chowd
 */

#ifndef PEER_H_
#define PEER_H_

#include <string.h>
#include "../protocol/protocol.h"
#include "../ds/ip_address.h"

class Peer
{
	int peer_id;
	int code_word;
	IPAddress ip_address;
	string host_name;
	int name_range_start;
	int name_range_end;
	double alpha;
	int k;
	ABSProtocol* protocol;

public:
	Peer();
	~Peer();

	double getAlpha()
	{
		return alpha;
	}

	void setAlpha(double alpha)
	{
		this->alpha = alpha;
	}

	int getCodeWord()
	{
		return code_word;
	}

	void setCodeWord(int codeWord)
	{
		code_word = codeWord;
	}

	IPAddress getIpAddress()
	{
		return ip_address;
	}
	int getIpAddress()
	{
		return ip_address.getIpAddress();
	}
	string getStrIpAddress()
	{
		return ip_address.getStrIpAddress();
	}

	void setIpAddress(char* ipAddress)
	{
		ip_address.setIp(ipAddress);
	}

	int getK()
	{
		return k;
	}

	void setK(int k)
	{
		this->k = k;
	}

	int getNameRangeEnd()
	{
		return name_range_end;
	}

	void setNameRangeEnd(int nameRangeEnd)
	{
		name_range_end = nameRangeEnd;
	}

	int getNameRangeStart()
	{
		return name_range_start;
	}

	void setNameRangeStart(int nameRangeStart)
	{
		name_range_start = nameRangeStart;
	}

	int getPeerId()
	{
		return peer_id;
	}

	void setPeerId(int peerId)
	{
		peer_id = peerId;
	}

	void setHostName(string host_name)
	{
		this->host_name = host_name;
	}

	string getHostName()
	{
		return host_name;
	}
};


#endif /* PEER_H_ */
