/*
 * CS 454 Assignment 3
 * July 13, 2012
 * Group Member 1: Evgeny Shulyatyev (ID 20242378)
 * Group Member 2: Shihabur Rahman Chowdhury (ID 20453766)
 * */


#include "server.h"
#include "comms.h"
#include "rpc.h"
#include "marshall.h"
#include "errors.h"

#include <stdio.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <memory.h>
#include <netdb.h>
#include <ctype.h>
#include <queue>
#include <string>
#include <pthread.h>
#include <set>
#include <algorithm>



using namespace std;

//GLOBAL VARIABLES
//char receiveBuffer[MAX_BUFFER_SIZE + 1];


int clientsSocketDescriptor = -1;
int binderSocketDescriptor = -1;
int proceduresRegistered = 0;
vector <long> registeredProcedureList;
pthread_mutex_t connectionLock;
pthread_mutex_t threadListLock;
fd_set connectionPool;
set<pthread_t> currentThreads;


//prototypes:
void executeMessageHandler(int connectionID, char *data);
void terminateServer();



int checkInitCondition()
{
	//pass/fail init condition
	if(clientsSocketDescriptor == -1 || binderSocketDescriptor == -1)
	{
		//this is an error - rpcRegister was called before rpcInit!
		return ERROR_NO_CONNECTIONS_INITIALIZED;
	}
	else
	{
		//everything's ok
		return 0;
	}
}

int checkProceduresRegisteredCondition()
{
	int checkInitResult = checkInitCondition();

	if(checkInitResult != 0)
		return checkInitResult;

	if(proceduresRegistered == 0)
	{
		//this is an error = rpcRegister has never successfully terminated
		return ERROR_NO_PROCEDURES_REGISTERED;
	}
	else
	{
		//everything's ok
		return 0;
	}
}


int rpcInit(void)
{
	char *binderAddress = 0;
	char *binderPort = 0;

	//Initialize the state variables
	registeredProcedureList.clear();
	proceduresRegistered = 0;

	//first, create a and initialize socket to listen to incoming client requests:
	clientsSocketDescriptor = connectServerSocket(0);

	//make sure we initialized the socket and binded correctly:
	if(clientsSocketDescriptor < 0)
		return ERROR_SERVER_SOCKET_INIT;

	//Retrieve Binder information from environment variables:
	binderAddress = getBinderAddress();
	binderPort = getBinderPort();

	if(binderAddress == NULL)
		return ERROR_SERVER_RPCINIT_BINDER_ENVIRONMENT_VARIABLE_NOT_SET;
	if(binderPort == NULL)
		return ERROR_SERVER_RPCINIT_BINDER_ENVIRONMENT_VARIABLE_NOT_SET;

	//secondly, establish connection with the binder:
	binderSocketDescriptor = createClientSocket(binderAddress, binderPort);

	//make sure we established binder connection properly
	if(binderSocketDescriptor < 0)
		return ERROR_SERVER_BINDER_CONNECTION_FAILED;

	//all is good - return success
	return GENERAL_SUCCESS_CODE;
}

int rpcRegister(char *name, int *argTypes, skeleton f)
{
	int dataLength = 0;
	int returnValue = 0;
	char messageType = 0;
	int receiveCode = 0;

	//initial check - make sure rpcInit was previously called and successfully completed
	int checkInitResult = checkInitCondition();
	if(checkInitResult != 0)
		return checkInitResult;

	RegisterMessage registerMessage;
	registerMessage.hostname = getLocalHostName();
	registerMessage.port = getPortNumber(clientsSocketDescriptor);
	registerMessage.name = name;
	registerMessage.argTypes = argTypes;
	registerMessage.functionAddress = (long) f;
	//printf("F = %ld\n", registerMessage.functionAddress);

	char *data = marshallRegisterMessage(registerMessage, &dataLength);

	//send the registration message to binder:
	int sendCode = send(binderSocketDescriptor, data, dataLength, 0);

	//delete data:
	if(data != NULL)
		delete[] data;

	//make sure we sent properly:
	if(sendCode < 0)
		return ERROR_SERVER_SENDING_REGISTER_MESSAGE;

	//wait for response
	char *receivedData = receiveMessage(binderSocketDescriptor, &receiveCode);
	//make sure we received properly
	if(receiveCode < 1)
		return ERROR_SERVER_RECEIVING_REGISTRATION_CONFIRMATION_MESSAGE;

	//determine what kind of a message is it:
	extractCharFromCharArray(4, receivedData, &messageType);

	if(messageType == REGISTER_SUCCESS_MSG_TYPE)
	{
		RegisterSuccessMessage successMessage = unmarshallRegisterSuccessMessage(receivedData + 5);
		returnValue = successMessage.reasonCode;
		//we now have at least one successfully initialized procedure - indicate that for future use within rpcExecute
		proceduresRegistered = 1;
		//add the registered procedure's address to the registeredProcedureList
		registeredProcedureList.push_back(registerMessage.functionAddress);
	}
	else if(messageType == REGISTER_FAILURE_MSG_TYPE)
	{
		RegisterFailureMessage failureMessage = unmarshallRegisterFailureMessage(receivedData + 5);
		returnValue = failureMessage.reasonCode;
	}
	else
	{
		return ERROR_SERVER_RECEIVED_UNKNOWN_MESSAGE_TYPE_FROM_BINDER;
	}


	//delete receivedData
	if(receivedData != NULL)
		delete[] receivedData;

	return returnValue;
}


int rpcExecute(void)
{

	//initial check: make sure rpcInit was previously run and at least on rpcRegister successfully completed
	int checkProcsRegisteredResult = checkProceduresRegisteredCondition();
	if(checkProcsRegisteredResult != 0)
		return checkProcsRegisteredResult;


	bool terminate = false;
	int receiveCode = 0;
	//initialize the connection lock
	pthread_mutex_init(&connectionLock, NULL);
	//initialize the thread list lock
	pthread_mutex_init(&threadListLock, NULL);


	fd_set readFds;
	int fdMax = 0;
	int connectionDescriptor;

	FD_ZERO(&connectionPool);
	FD_ZERO(&readFds);

	FD_SET(clientsSocketDescriptor, &connectionPool);
	FD_SET(binderSocketDescriptor, &connectionPool);

	fdMax = max(clientsSocketDescriptor, binderSocketDescriptor);

	while(!terminate)
	{
		readFds = connectionPool;
		int nSelect = select(fdMax + 1, &readFds, NULL, NULL, NULL);

		if(nSelect < 0)
		{
			return ERROR_SERVER_SELECT_SYSCALL_FAILED;
		}

		for(int i = 0; i <= fdMax; i++)
		{

			char *receivedData = NULL;

			if(FD_ISSET(i, &readFds))
			{
				if(clientsSocketDescriptor == i)
				{
					//This is a new connection from the client
					connectionDescriptor = accept(clientsSocketDescriptor, NULL, NULL);
					if(connectionDescriptor < 0)
					{
						return ERROR_SERVER_UNABLE_ACCEPT_NEW_CLIENT_CONNECTION;
					}

					//New connection is accepted:
					//CRITICAL SECTION
					pthread_mutex_lock(&connectionLock);

					FD_SET(connectionDescriptor, &connectionPool);
					if(connectionDescriptor > fdMax)
						fdMax = connectionDescriptor;

					pthread_mutex_unlock(&connectionLock);
					//END OF CRITICAL SECTION
				}
				else if(binderSocketDescriptor == i)
				{
					//this message is coming from the binder - should be terminate message
					receivedData = receiveMessage(binderSocketDescriptor, &receiveCode);

					if(receiveCode < 0)
					{
						//this signals an error
						return ERROR_SERVER_FAILED_BINDER_MESSAGE_RECEIVE;
					}
					else if(receiveCode == 0)
					{
						//binder is closing the connection
						//CRITICAL SECTION
						pthread_mutex_lock(&connectionLock);
						close(i);
						FD_CLR(i, &connectionPool);
						pthread_mutex_unlock(&connectionLock);
						//END OF CRITICAL SECTION
					}
					else
					{
						//A new incoming message is received from the binder, process it
						int dataLength = 0;
						char msgType = 0;

						//strip out the length:
						extractIntFromCharArray(0, receivedData, &dataLength);
						//determine what kind of a message is it:
						extractCharFromCharArray(4, receivedData, &msgType);

						switch(msgType)
						{
							case TERMINATE_MSG_TYPE:
								terminate = true;
								terminateServer();
								break;
						}
					}
				}
				else
				{
					//Client is sending a request!:
					char *receivedData = receiveMessage(i, &receiveCode);

					if(receiveCode < 0)
					{
						//this signals an error
						return ERROR_SERVER_FAILED_CLIENT_REQUEST_RECEIVE;
					}
					else if(receiveCode == 0)
					{
						//Client is closing the connection
						//CRITICAL SECTION
						pthread_mutex_lock(&connectionLock);
						close(i);
						FD_CLR(i, &connectionPool);
						pthread_mutex_unlock(&connectionLock);
						//END OF CRITICAL SECTION
					}
					else
					{
						//A new incoming client message is received, process it
						int dataLength = 0;
						char msgType = 0;
						int currentIndex = 0;
						currentIndex = extractIntFromCharArray(currentIndex, receivedData, &dataLength);
						currentIndex = extractCharFromCharArray(currentIndex, receivedData, &msgType);

						switch(msgType)
						{
							case EXECUTE_MSG_TYPE:
								executeMessageHandler(i, receivedData + currentIndex);
								break;
						}
					}
				}
			}

			if(receivedData != NULL)
				delete[] receivedData;

			if(terminate)
				break;
		}
	}

	return GENERAL_SUCCESS_CODE;

}

void terminateServer()
{
	//terminate msg has been received
	//join all currently processing threads and then exit
	while(true)
	{
		//lock and check if the thread list is empty
		pthread_t waitingThread;

		pthread_mutex_lock(&threadListLock);

		if(!currentThreads.empty())
		{
			//get the remaining thread
			waitingThread = *currentThreads.begin();
			//unlock
			pthread_mutex_unlock(&threadListLock);
			//join
			pthread_join(waitingThread, NULL);
		}
		else
		{
			//no more threads are waiting
			//let's be clean = unlock:
			pthread_mutex_unlock(&threadListLock);

			//close the binder connection
			close(binderSocketDescriptor);
			FD_CLR(binderSocketDescriptor, &connectionPool);

			//break out and proceed with termination
			break;
		}
	}
}

void *messageExecutionThread(void *threadArgData)
{
	char *marshalledReplyMessage;
	int dataLength;

	ExecutionThreadArgData *argData = (ExecutionThreadArgData *) threadArgData;
	int connectionID = argData->connectionID;
	char *data = argData->data;
	pthread_t currentThread = pthread_self(); //argData->executionThread;

	//first, add the current thread to the list of current threads:
	//lock it, because it is a critical section:
	pthread_mutex_lock(&threadListLock);
	currentThreads.insert(currentThread);
	pthread_mutex_unlock(&threadListLock);

	//unmarshall and execute the message
	ExecuteMessage executeMessage = unmarshallExecuteMessage(data);

	int result;
	int i;
	for(i = 0; i < registeredProcedureList.size(); i++)
		if(registeredProcedureList[i] == executeMessage.functionAddress)
			break;

	//Change for rpcCacheCall, previous functionality not altered
	if(i >= registeredProcedureList.size())
		result = ERROR_SERVER_FUNCTION_DOES_NOT_EXIST;

	else
	{
		skeleton functionPointer = (skeleton) executeMessage.functionAddress;
		result = (functionPointer)(executeMessage.argTypes, executeMessage.args);
	}

	//check if the execution was successful:
	if(result >= 0)
	{
		//Create execute success message:
		ExecuteSuccessMessage successMessage;
		successMessage.argTypes = executeMessage.argTypes;
		successMessage.args = executeMessage.args;
		successMessage.argsLength = executeMessage.argsLength;
		successMessage.name = executeMessage.name;
		marshalledReplyMessage = marshallExecuteSuccessMessage(successMessage, &dataLength);
	}
	else
	{
		//execution failed:
		ExecuteFailureMessage failureMessage;
		failureMessage.reasonCode = result;
		marshalledReplyMessage = marshallExecuteFailureMessage(failureMessage, &dataLength);
	}

	//send the marshalled reply message back on connection ID:
	int sendCode = send(connectionID, marshalledReplyMessage, dataLength, 0);

	//delete sent data:
	if(marshalledReplyMessage != NULL)
		delete[] marshalledReplyMessage;

	//close the connection:
	//CRITICAL SECTION
	pthread_mutex_lock(&connectionLock);

	FD_CLR(connectionID, &connectionPool);
	int closeResult = close(connectionID);

	pthread_mutex_unlock(&connectionLock);
	//END OF CRITICAL SECTION

	//delete the data from the heap:
	delete argData;

	//request has been served, the thread is about to terminate, remove itself from the list
	pthread_mutex_lock(&threadListLock);
	currentThreads.erase(currentThread);
	//printf("Set size = %d, Current Thread = %u\n", currentThreads.size(), (unsigned int)currentThread);
	pthread_mutex_unlock(&threadListLock);
}

void executeMessageHandler(int connectionID, char *data)
{
	//first, unmarshall the execute message:
	pthread_t msgExecuteThread;

	//pass arguments to the new thread
	ExecutionThreadArgData* argData = new ExecutionThreadArgData;
	argData->connectionID = connectionID;
	argData->data = data;

	//execute the thread
	pthread_create(&msgExecuteThread, NULL, messageExecutionThread, (void *) argData);

}




