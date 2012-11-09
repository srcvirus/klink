/*
 * comms.h
 *
 *  Created on: Jul 6, 2012
 *      Author: flexuser
 */
/*
 * CS 454 Assignment 3
 * July 13, 2012
 * Group Member 1: Evgeny Shulyatyev (ID 20242378)
 * Group Member 2: Shihabur Rahman Chowdhury (ID 20453766)
 * */




#ifndef COMMS_H_
#define COMMS_H_

#define BINDER_ADDRESS "BINDER_ADDRESS"
#define BINDER_PORT	   "BINDER_PORT"
#define LENGTH_BUFFER_SIZE 4

#include <string>

using namespace std;

//prototypes:

char *receiveMessage(int socketDescriptor, int *receiveCode);
int createServerSocket();
int createClientSocket(char *serverName, char *serverPort);
int bindToSocket(int socketDescriptor, int portNumber);
int listenToSocket(int socketDescriptor, int connections);
int connectServerSocket(int portNumber);
int acceptConnection(int socketDescriptor);
void printSocketInfo(int socketDescriptor);
void printErrorMessage(int errorCode);
string getLocalHostName();
int getPortNumber(int socketDescriptor);
char *getBinderAddress();
char *getBinderPort();

#endif /* COMMS_H_ */
