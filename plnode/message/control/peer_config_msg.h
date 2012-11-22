/*
 * peer_config_msg.h
 *
 *  Created on: 2012-11-22
 *      Author: sr2chowd
 */

#ifndef PEER_CONFIG_MSG_H_
#define PEER_CONFIG_MSG_H_

#include "../message.h"

class PeerConfigMessage:public ABSMessage
{
	int parameter_k;
	double parameter_alpha;

public:
	PeerConfigMessage():ABSMessage(){;}
	void setParameterK(int k)
	{
		parameter_k = k;
	}

	int getParameterK()
	{
		return parameter_k;
	}

	void setParameterAlpha(double alpha)
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
		*serialize_length = sizeof(PeerConfigMessage());
		return buffer;
	}

	virtual ABSMessage* deserialize(char* buffer, int buffer_length)
	{
		PeerConfigMessage* pcfg_message = new PeerConfigMessage();
		memcpy(&pcfg_message, buffer, buffer_length);
		return &pcfg_message;
	}
};


#endif /* PEER_CONFIG_MSG_H_ */
