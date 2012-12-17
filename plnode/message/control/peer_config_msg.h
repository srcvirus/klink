/*
 * peer_config_msg.h
 *
 *  Created on: 2012-11-22
 *      Author: sr2chowd
 */

#ifndef PEER_CONFIG_MSG_H_
#define PEER_CONFIG_MSG_H_

#include "../message.h"
#include <memory.h>

class PeerConfigMessage:public ABSMessage
{
	int parameter_k;
	double parameter_alpha;

public:
	PeerConfigMessage():ABSMessage()
	{
		messageType = MSG_PEER_CONFIG;
	}

	void setK(int k)
	{
		parameter_k = k;
	}

	int getK()
	{
		return parameter_k;
	}

	void setAlpha(double alpha)
	{
		parameter_alpha = alpha;
	}

	double getAlpha()
	{
		return parameter_alpha;
	}

	virtual char* serialize(int* serialize_length)
	{
		char* buffer = new char[sizeof(PeerConfigMessage)];
		memcpy(buffer, (char*)(this), sizeof(PeerConfigMessage));
		*serialize_length = sizeof(PeerConfigMessage);
		return buffer;
	}

	virtual ABSMessage* deserialize(char* buffer, int buffer_length)
	{
		memcpy(this, buffer, buffer_length);
		return this;
	}

	virtual void message_print_dump()
	{
		puts("<<<<<<<<<<<<<<<<<<<<>>>>>>>>>>>>>>>>>>>>");
		printf("Configuration Message\n");
		ABSMessage::message_print_dump();
		printf("Parameter K = %d\n", parameter_k);
		printf("Parameter alpha = %.2lf\n", parameter_alpha);
		puts("<<<<<<<<<<<<<<<<<<<<>>>>>>>>>>>>>>>>>>>>");
	}
};


#endif /* PEER_CONFIG_MSG_H_ */
