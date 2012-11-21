/*
 * peer.h
 *
 *  Created on: 2012-11-21
 *      Author: sr2chowd
 */

#ifndef PEER_H_
#define PEER_H_

#include <string.h>

class Peer
{
	int peer_id;
	int code_word;
	int ip_address;
	int name_range_start;
	int name_range_end;
	double alpha;
	int k;

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

	int getIpAddress()
	{
		return ip_address;
	}

	void setIpAddress(int ipAddress)
	{
		ip_address = ipAddress;
	}

	void setIpAddress(char* ipAddress)
	{
		ip_address = 0;
		int to_shift = 24;
		int ip_component = 0;

		char* ptr = strtok(ipAddress, ".");

		if(ptr != NULL)
		{
			ip_component = atoi(ptr);
			ip_address |= (ip_component << to_shift);
			to_shift -= 8;
		}

		while((ptr = strtok(NULL, " ")) != NULL)
		{
			ip_component = atoi(ptr);
			ip_address |= (ip_component << to_shift);
			to_shift -= 8;
		}
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
};


#endif /* PEER_H_ */
