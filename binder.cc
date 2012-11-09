/*
 * CS 454 Assignment 3
 * July 13, 2012
 * Group Member 1: Evgeny Shulyatyev (ID 20242378)
 * Group Member 2: Shihabur Rahman Chowdhury (ID 20453766)
 * */

#include <stdio.h>
#include <map>
#include <vector>
#include <ifaddrs.h>
#include <sys/types.h>
#include <stdlib.h>
#include <set>
#include <string.h>
#include "comms.h"
#include "marshall.h"
#include "errors.h"
#include "database.h"

using namespace std;

#define MAX_CONNECTIONS 10
#define BINDER_PORT_NUMBER 0
LocationDatabase locationDB;
set <int> serverConnectionSet;

void registerMessageHandler(int connectionHandler, char* message);
void locationRequestMessageHandler(int connectionHandler, char* message);
void terminationMessageHandler(int connectionHandler, char* message);
void locationCacheRequestMessageHandler(int connectionHandler, char* message);

int main()
{
	bool terminate = false;

	fd_set connectionPool;
	fd_set readFds;
	int fdMax = 0, i;
	int connectionDescriptor, socketDescriptor;

	FD_ZERO(&connectionPool);
	FD_ZERO(&readFds);

	socketDescriptor = connectServerSocket(BINDER_PORT_NUMBER);
	printSocketInfo(socketDescriptor);


	FD_SET(socketDescriptor, &connectionPool);
	fdMax = socketDescriptor;
	serverConnectionSet.clear();
	locationDB.clear();

	while(!terminate)
	{
		readFds = connectionPool;
		int nSelect = select(fdMax + 1, &readFds, NULL, NULL, NULL);

		if(nSelect < 0)
		{
			//printf("Error\n");
			exit(1);
		}

		for(i = 0; i <= fdMax; i++)
		{
			if(FD_ISSET(i, &readFds))
			{
				if(socketDescriptor == i)
				{
					connectionDescriptor = accept(socketDescriptor, NULL, NULL);
					if(connectionDescriptor < 0)
					{
						//printErrorMessage(connectionDescriptor);
						continue;
					}
					FD_SET(connectionDescriptor, &connectionPool);
					if(connectionDescriptor > fdMax)
						fdMax = connectionDescriptor;
						//printf("Connection From %d", connectionDescriptor);
				}
				else
				{
					int size = 0;
					char *buf = receiveMessage(i, &size);
					if(size == 0)
					{
						close(i);
						serverConnectionSet.erase(i);
						locationDB.removeLocation(i);
						FD_CLR(i, &connectionPool);
						//puts("Connection Closed");
					}
					else
					{
						int length;
						char msgType;
						int offset = 0;
						offset = extractIntFromCharArray(offset, buf, &length);
						offset = extractCharFromCharArray(offset, buf, &msgType);

						switch(msgType)
						{
							case REGISTER_MSG_TYPE:
								registerMessageHandler(i, buf + offset);
								break;

							case LOC_REQUEST_MSG_TYPE:
								locationRequestMessageHandler(i, buf + offset);
								break;
							case TERMINATE_MSG_TYPE:
								terminationMessageHandler(i, buf + offset);
								terminate = true;
								close(socketDescriptor);
								break;
							case LOCATION_CACHE_REQUEST_MESSAGE_TYPE:
								locationCacheRequestMessageHandler(i, buf + offset);
								break;
						}

						delete[] buf;
					}
				}
			}
			if(terminate)
				break;
		}
	}

	fdMax = 0;
	FD_ZERO(&connectionPool);

	for(set <int> ::iterator pp = serverConnectionSet.begin(); pp != serverConnectionSet.end(); pp++)
	{
		FD_SET(*pp, &connectionPool);
		if(fdMax < *pp)
			fdMax = *pp;
	}

	while(!serverConnectionSet.empty())
	{
		readFds = connectionPool;
		int nSelect = select(fdMax + 1, &readFds, NULL, NULL, NULL);

		if(nSelect < 0)
		{
			exit(1);
		}

		for(i = 0; i <= fdMax; i++)
		{
			if(FD_ISSET(i, &readFds))
			{
				int size = 0;
				char *buf = receiveMessage(i, &size);
				if(size == 0)
				{
					close(i);
					serverConnectionSet.erase(i);
					FD_CLR(i, &connectionPool);
				}
			}
		}
	}

	//puts("BINDER TERMINATED");
	return 0;
}



void registerMessageHandler(int connectionHandler, char* message)
{
	RegisterMessage regMsg;
	RegisterSuccessMessage regSuccMsg;
	char* retMessage = NULL;
	int retMessageLength = 0;
	string key;
	Location loc;

	//puts("REGISTER MSG");

	serverConnectionSet.insert(connectionHandler);

	regMsg = unmarshallRegisterMessage(message);
	key = hash(regMsg.name.c_str(), regMsg.argTypes, regMsg.argsLength);

	loc.hostName = regMsg.hostname;
	loc.port = regMsg.port;
	loc.skeletonAddress = regMsg.functionAddress;
	loc.connectionIdentifier = connectionHandler;

	regSuccMsg.reasonCode = locationDB.addLocation(key, loc);
	retMessage = marshallRegisterSuccessMessage(regSuccMsg, &retMessageLength);

	send(connectionHandler, retMessage, retMessageLength, 0);
	delete[] retMessage;
}

void locationRequestMessageHandler(int connectionHandler, char* message)
{
	//puts("LOC_REQ_MSG");

	LOC_RequestMessage locReqMsg;
	LOC_SuccessMessage locSuccMsg;
	LOC_FailureMessage locFailMsg;

	string key;
	Location loc;
	char* retMessage;
	int retMessageLength;

	locReqMsg = unmarshallLOC_RequestMessage(message);
	key = hash(locReqMsg.name.c_str(), locReqMsg.argTypes, locReqMsg.argsLength);
	loc = locationDB.getLocation(key);

	if(loc.hostName != "" && loc.port > 0)
	{
		locSuccMsg.hostname = loc.hostName;
		locSuccMsg.port = loc.port;
		locSuccMsg.functionAddress = loc.skeletonAddress;

		retMessage = marshallLOC_SuccessMessage(locSuccMsg, &retMessageLength);
		send(connectionHandler, retMessage, retMessageLength, 0);
		delete[] retMessage;
	}
	else
	{
		locFailMsg.reasonCode = ERROR_NO_SERVER_FOUND;
		retMessage = marshallLOC_FailureMessage(locFailMsg, &retMessageLength);
		send(connectionHandler, retMessage, retMessageLength, 0);
		delete[] retMessage;
	}
}

void terminationMessageHandler(int connectionHandler, char* message)
{
	TerminateMessage terminateMsg;
	int retMessageLength;
	char* retMessage = marshallTerminateMessage(terminateMsg, &retMessageLength);

	for(set <int> ::iterator pp = serverConnectionSet.begin(); pp != serverConnectionSet.end(); pp++)
	{
		int serverConnectionHandler = *pp;
		send(serverConnectionHandler, retMessage, retMessageLength, 0);
	}

	delete[] retMessage;
}

void locationCacheRequestMessageHandler(int connectionHandler, char* message)
{
	LOC_CacheRequestMessage locCacheReqMsg;
	LOC_CacheSuccessMessage locCacheSuccMsg;
	LOC_CacheFailMessage locCacheFailMsg;

	string key;
	Location loc;
	char* retMessage;
	int retMessageLength;

	locCacheReqMsg = unmarshallLOCCacheRequestMessage(message);
	key = hash(locCacheReqMsg.name.c_str(), locCacheReqMsg.argTypes, locCacheReqMsg.argsLength);
	vector <Location> vLoc = locationDB.getAllLocations(key);

	if(vLoc.empty())
	{
		locCacheFailMsg.reasonCode = ERROR_NO_SERVER_FOUND;
		retMessage = marshalLOCCacheFailMessage(locCacheFailMsg, &retMessageLength);
		send(connectionHandler, retMessage, retMessageLength, 0);
		delete[] retMessage;
	}
	else
	{
		locCacheSuccMsg.hostnames.clear();
		locCacheSuccMsg.ports.clear();
		locCacheSuccMsg.functionAddresses.clear();
		locCacheSuccMsg.responseSize = vLoc.size();

		for(int i = 0; i < locCacheSuccMsg.responseSize; i++)
		{
			locCacheSuccMsg.hostnames.push_back(vLoc[i].hostName);
			locCacheSuccMsg.ports.push_back(vLoc[i].port);
			locCacheSuccMsg.functionAddresses.push_back(vLoc[i].skeletonAddress);
		}
		retMessage = marshalLOCCacheSuccessMessage(locCacheSuccMsg, &retMessageLength);
		int sendCode = send(connectionHandler, retMessage, retMessageLength, 0);
		delete[] retMessage;
	}
}


