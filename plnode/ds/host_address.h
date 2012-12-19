#ifndef HOST_ADDRESS_H
#define HOST_ADDRESS_H

#include <string>

using namespace std;

class HostAddress
{
private:
	string hostName;
	int hostPort;
public:

	HostAddress()
	{
	}

	HostAddress(string name, int port)
	{
		hostName = name;
		hostPort = port;
	}

	void SetHostPort(int hostPort)
	{
		this->hostPort = hostPort;
	}

	int GetHostPort() const
	{
		return hostPort;
	}

	void SetHostName(string hostName)
	{
		this->hostName = hostName;
	}

	string GetHostName() const
	{
		return hostName;
	}
};
#endif
