/*
 * CS 454 Assignment 3
 * July 13, 2012
 * Group Member 1: Evgeny Shulyatyev (ID 20242378)
 * Group Member 2: Shihabur Rahman Chowdhury (ID 20453766)
 * */

#include "rpc.h"
#include "database.h"
#include "comms.h"
#include "marshall.h"
#include "errors.h"
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define ENV_BINDER_ADDRESS "BINDER_ADDRESS"
#define ENV_BINDER_PORT	   "BINDER_PORT"

LocationDatabase locationCache;

int executeRpcCall(int serverConnectionHandler, ExecuteMessage execMessage, int* argTypes, void** originalArgs)
{
	int retCode = GENERAL_SUCCESS_CODE;
	int dataLength = 0;
	char messageType;
	char *data = marshallExecuteMessage(execMessage, &dataLength);

	/*send the byte array to the server*/
	int sendLength = sendto(serverConnectionHandler, data, dataLength, 0, NULL, 0);
	delete[] data;

	if(sendLength < 0)
	{
		retCode = ERROR_SERVER_REQUEST_SEND_FAIL;
		return retCode;
	}

	/*receive response from the server*/
	data = receiveMessage(serverConnectionHandler, &dataLength);

	if(dataLength <= 0)
	{
		/*set retCode to indicate some error occured while receving the execution results from server*/
		retCode = ERROR_SERVER_CONNECTION_CLOSED;
		return retCode;
	}

	/*Extract the parameters from the message and place them in the args array*/
	int offset = 0;
	offset = extractIntFromCharArray(offset, data, &dataLength);
	offset = extractCharFromCharArray(offset, data, &messageType);

	switch(messageType)
	{
	case EXECUTE_FAILURE_MSG_TYPE:
		ExecuteFailureMessage execFailMsg;
		execFailMsg = unmarshallExecuteFailureMessage(data + offset);
		retCode = execFailMsg.reasonCode;
		break;

	case EXECUTE_SUCCESS_MSG_TYPE:
		ExecuteSuccessMessage execSuccessMsg;
		execSuccessMsg = unmarshallExecuteSuccessMessage(data + offset);
		/*copy the return values into args*/
		for(int i = 0; i < execSuccessMsg.argsLength; i++)
		{
			int arraySize = countArrayElements(argTypes[i]);
			int type = findArgType(argTypes[i]);
			void* currentArg = originalArgs[i];

			switch(type)
			{
				case ARG_CHAR:
					for(int j = 0; j < arraySize; j++)
						*((char*)currentArg + j) =  *((char*)execSuccessMsg.args[i] + j);
					break;
				case ARG_SHORT:
					for(int j = 0; j < arraySize; j++)
						*((short*)currentArg + j) =  *((short*)execSuccessMsg.args[i] + j);
					break;
				case ARG_INT:
					for(int j = 0; j < arraySize; j++)
						*((int*)currentArg + j) =  *((int*)execSuccessMsg.args[i] + j);
					break;
				case ARG_LONG:
					for(int j = 0; j < arraySize; j++)
						*((long*)currentArg + j) =  *((long*)execSuccessMsg.args[i] + j);
					break;
				case ARG_DOUBLE:
					for(int j = 0; j < arraySize; j++)
						*((double*)currentArg + j) =  *((double*)execSuccessMsg.args[i] + j);
					break;
				case ARG_FLOAT:
					for(int j = 0; j < arraySize; j++)
						*((float*)currentArg + j) =  *((float*)execSuccessMsg.args[i] + j);
					break;
			}
		}
		break;
	}

	close(serverConnectionHandler);
	delete[] data;
	return retCode;
}

int rpcCall(char* name, int* argTypes, void** args)
{
	int retCode = GENERAL_SUCCESS_CODE;
	int sendLength = 0;

	/*get binder's host name*/
	char* binderHostName = getenv(ENV_BINDER_ADDRESS);

	/*get binder's port number*/
	char* binderPortNumber = getenv(ENV_BINDER_PORT);

	/*create a socket to connect to the binder*/
	int binderConnectionHandler = createClientSocket(binderHostName, binderPortNumber);

	if(binderConnectionHandler < 0)
	{
		/*set retCode to indicate a connection to binder error has occurred*/
		retCode = ERROR_BINDER_CONNECT_FAIL;
		return retCode;
	}

	/*request for a server location from the binder*/
	LOC_RequestMessage locReqMsg;
	int argLength = calculateArgsTypesLength(argTypes);
	locReqMsg.argsLength = argLength;
	locReqMsg.argTypes = argTypes;
	locReqMsg.name = name;

	int dataLength = 0;
	char* data = marshallLOC_RequestMessage(locReqMsg, &dataLength);

	sendLength = sendto(binderConnectionHandler, data, dataLength, 0, NULL, 0);
	//sendLength = send(binderConnectionHandler, data, dataLength, 0);
	delete[] data;
	if(sendLength < 0)
	{
		/*set retCode to indicate a connection to binder error has occured*/
		retCode = ERROR_BINDER_REQUEST_SEND_FAIL;
		return retCode;
	}
	data = receiveMessage(binderConnectionHandler, &dataLength);

	if(dataLength <= 0)
	{
		/*set retCode to indicate that error occurred while receiving server info from binder*/
		retCode = ERROR_BINDER_CONNECTION_CLOSED;
		return retCode;
	}

	/*extract the server location*/
	int offset = 0;
	char messageType;

	char* serverHostName;
	char* portNumber;
	int skeletonAddress;

	offset = extractIntFromCharArray(offset, data, &dataLength);
	offset = extractCharFromCharArray(offset, data, &messageType);
	switch(messageType)
	{
		case LOC_FAILURE_MSG_TYPE:
			LOC_FailureMessage locFailMsg;
			locFailMsg = unmarshallLOC_FailureMessage(data + offset);
			retCode = locFailMsg.reasonCode;
			break;

		case LOC_SUCCESS_MSG_TYPE:
			LOC_SuccessMessage locSuccMsg;
			locSuccMsg = unmarshallLOC_SuccessMessage(data + offset);

			/*extract host name, port number and skeleton address from the message*/
			serverHostName = new char[locSuccMsg.hostname.size() + 1];
			strcpy(serverHostName, locSuccMsg.hostname.c_str());

			/*extract port number
			Find number of digits in the locaSuccMessage.port*/
			int nDigits = 0;
			int tempPort = locSuccMsg.port;
			while(tempPort)
			{
				tempPort /= 10;
				nDigits++;
			}

			/*Allocate nDigit spaces for writing the port as a string into portNumber*/
			portNumber = new char[nDigits + 1];
			sprintf(portNumber, "%d", locSuccMsg.port);

			/*extract skeleton address*/
			skeletonAddress = locSuccMsg.functionAddress;
			break;
	}

	delete[] data;

	if(retCode < 0)
		return retCode;

	close(binderConnectionHandler);

	/*create a socket to the server*/
	int serverConnectionHandler = createClientSocket(serverHostName, portNumber);

	if(serverConnectionHandler < 0)
	{
		/*set retCode to indicate a connection to the server could not be established*/
		retCode = ERROR_SERVER_CONNECT_FAIL;
		return retCode;
	}

	/*pack the arguments in a byte array*/
	ExecuteMessage execMessage;
	execMessage.name = name;
	execMessage.argTypes = argTypes;
	execMessage.argsLength = argLength;
	execMessage.args = args;
	execMessage.functionAddress = skeletonAddress;

	retCode = executeRpcCall(serverConnectionHandler, execMessage, argTypes, args);

	delete[] serverHostName;
	delete[] portNumber;

	return retCode;
}

int rpcCacheCall(char* name, int* argTypes, void** args)
{
	int i;
	int retCode = 0;
	int serverConnectionHandler;
	int skeletonAddress;
	int offset, dataLength, sendLength;
	char* data;
	bool cacheHit = false;
	char messageType;

	//check the local cache
	string functionName = name;
	int argsLength = calculateArgsTypesLength(argTypes);
	string key = hash(functionName.c_str(), argTypes, argsLength);

	vector <Location> cachedServerLocations = locationCache.getAllLocations(key);
	//if there is cache hit try all the servers to establish a connection.
	if(!cachedServerLocations.empty())
	{
		for(i = 0; i < (int)cachedServerLocations.size(); i++)
		{
			string host = cachedServerLocations[i].hostName;
			char* serverHostName = new char[host.size() + 1];
			strcpy(serverHostName, host.c_str());

			int nDigits = 0;
			int tempPort = cachedServerLocations[i].port;
			while(tempPort)
			{
				tempPort /= 10;
				nDigits++;
			}
			char* portNumber = new char[nDigits + 1];
			sprintf(portNumber, "%d", cachedServerLocations[i].port);

			serverConnectionHandler = createClientSocket(serverHostName, portNumber);
			delete[] serverHostName;
			delete[] portNumber;

			if(serverConnectionHandler > 0)
			{
				skeletonAddress = cachedServerLocations[i].skeletonAddress;
				ExecuteMessage execMessage;
				execMessage.name = name;
				execMessage.argTypes = argTypes;
				execMessage.argsLength = argsLength;
				execMessage.args = args;
				execMessage.functionAddress = skeletonAddress;
				retCode = executeRpcCall(serverConnectionHandler, execMessage, argTypes, args);
				if(retCode >= 0)
				{
					//puts("Cache hit");
					cacheHit = true;
					break;
				}
			}
		}
	}

	if(!cacheHit)
	{
		/*get binder's host name*/
		char* binderHostName = getenv(ENV_BINDER_ADDRESS);
		/*get binder's port number*/
		char* binderPortNumber = getenv(ENV_BINDER_PORT);

		/*create a socket to connect to the binder*/
		int binderConnectionHandler = createClientSocket(binderHostName, binderPortNumber);

		if(binderConnectionHandler < 0)
		{
			/*set retCode to indicate a connection to binder error has occured*/
			retCode = ERROR_BINDER_CONNECT_FAIL;
			return retCode;
		}

		/*send a cache location message to the binder*/
		LOC_CacheRequestMessage locCacheReqMsg;
		locCacheReqMsg.name = name;
		locCacheReqMsg.argsLength = argsLength;
		locCacheReqMsg.argTypes = argTypes;

		data = marshallLOCCacheRequestMessage(locCacheReqMsg, &dataLength);
		sendLength = send(binderConnectionHandler, data, dataLength, 0);
		delete[] data;
		if(sendLength < 0)
		{
			retCode = ERROR_BINDER_REQUEST_SEND_FAIL;
			return retCode;
		}

		/*extract the server locations from the message received from binder*/
		data = receiveMessage(binderConnectionHandler, &dataLength);
		if(dataLength <= 0)
		{
			retCode = ERROR_BINDER_CONNECTION_CLOSED;
			return retCode;
		}
		offset = 0;
		int msgLength;
		offset = extractIntFromCharArray(offset, data, &msgLength);
		offset = extractCharFromCharArray(offset, data, &messageType);

		LOC_CacheFailMessage locCacheFailMsg;

		switch(messageType)
		{
		case LOCATION_CACHE_REQUEST_FAIL_MESSAGE_TYPE:
			locCacheFailMsg = unmarshallLOCCacheFailMessage(data + offset);
			retCode = locCacheFailMsg.reasonCode;
			break;

		case LOCATION_CACHE_REQUEST_SUCCESS_MESSAGE_TYPE:
			/* put the server locations against the key in the cache */
			locationCache.removeLocation(key);
			LOC_CacheSuccessMessage locCacheSucMsg = unmarshallLOCCacheSuccessMessage(data + offset);
			Location loc;
			for(i = 0; i < locCacheSucMsg.responseSize; i++)
			{
				loc.hostName = locCacheSucMsg.hostnames[i];
				loc.port = locCacheSucMsg.ports[i];
				loc.skeletonAddress = locCacheSucMsg.functionAddresses[i];
				locationCache.addLocation(key, loc);
			}
			break;
		}

		delete[] data;
		if(retCode < 0)
			return retCode;

		/* close the connection to the binder */
		close(binderConnectionHandler);

		cachedServerLocations = locationCache.getAllLocations(key);
		for(i = 0; i < (int)cachedServerLocations.size(); i++)
		{
			string host = cachedServerLocations[i].hostName;
			char* serverHostName = new char[host.size() + 1];
			strcpy(serverHostName, host.c_str());

			int nDigits = 0;
			int tempPort = cachedServerLocations[i].port;
			while(tempPort)
			{
				tempPort /= 10;
				nDigits++;
			}
			char* portNumber = new char[nDigits + 1];
			sprintf(portNumber, "%d", cachedServerLocations[i].port);

			serverConnectionHandler = createClientSocket(serverHostName, portNumber);
			delete[] serverHostName;
			delete[] portNumber;

			if(serverConnectionHandler > 0)
			{
				skeletonAddress = cachedServerLocations[i].skeletonAddress;
				ExecuteMessage execMessage;
				execMessage.name = name;
				execMessage.args = args;
				execMessage.argsLength = argsLength;
				execMessage.argTypes = argTypes;
				execMessage.functionAddress = skeletonAddress;
				retCode = executeRpcCall(serverConnectionHandler, execMessage, argTypes, args);
				if(retCode >= 0)
					break;
			}
		}
		/* try to establish connection with one of the servers */
	}

	if(i >= (int)cachedServerLocations.size())
	{
		/* set retCode to indicate no server was found */
		retCode = ERROR_SERVER_CONNECT_FAIL;
		return retCode;
	}

	return retCode;
}

int rpcTerminate()
{
	/*create a socket to the binder*/
	int retCode = GENERAL_SUCCESS_CODE;

	/*get binder's host name*/
	char* binderHostName = getenv(ENV_BINDER_ADDRESS);

	/*get binder's port number*/
	char* binderPortNumber = getenv(ENV_BINDER_PORT);

	/*create a socket to connect to the binder*/
	int binderConnectionHandler = createClientSocket(binderHostName, binderPortNumber);

	if(binderConnectionHandler < 0)
	{
		/*set retCode to indicate a connection to binder error has occured*/
		retCode = ERROR_BINDER_CONNECT_FAIL;
		return retCode;
	}

	/*send a terminate request message*/
	TerminateMessage termMessage;
	int dataLength = 0;
	char* data = marshallTerminateMessage(termMessage, &dataLength);
	int sendLength = send(binderConnectionHandler, data, dataLength, 0);
	if(sendLength < 0)
	{
		retCode = ERROR_BINDER_REQUEST_SEND_FAIL;
		return retCode;
	}
	return retCode;
}



