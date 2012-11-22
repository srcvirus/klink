/*
 * message.h
 *
 *  Created on: 2012-11-21
 *      Author: sr2chowd
 */

#ifndef MESSAGE_H_
#define MESSAGE_H_

#include <string>

using namespace std;

class ABSMessage
{
	string dest_host;
	int dest_port;
	string source_host;
	int source_port;
	int overlay_hops;
	int overlay_ttl;
public:
	ABSMessage();
	virtual ~ABSMessage();

	virtual char* serialize() = 0;

	string getDestHost()
	{
		return dest_host;
	}

	void setDestHost(const string& destHost)
	{
		dest_host = destHost;
	}

	void setDestHost(const char* destHost)
	{
		dest_host = destHost;
	}

	int getDestPort()
	{
		return dest_port;
	}

	void setDestPort(int destPort)
	{
		dest_port = destPort;
	}

	int getOverlayHops()
	{
		return overlay_hops;
	}

	void setOverlayHops(int overlayHops)
	{
		overlay_hops = overlayHops;
	}

	int getOverlayTtl()
	{
		return overlay_ttl;
	}

	void setOverlayTtl(int overlayTtl)
	{
		overlay_ttl = overlayTtl;
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
};

#endif /* MESSAGE_H_ */
