/*
 * CS 454 Assignment 3
 * July 13, 2012
 * Group Member 1: Evgeny Shulyatyev (ID 20242378)
 * Group Member 2: Shihabur Rahman Chowdhury (ID 20453766)
 * */


#include <string>
#include <vector>
//#include "rpc.h"

using namespace std;

//Main message types
#define REGISTER_MSG_TYPE 0
#define REGISTER_SUCCESS_MSG_TYPE 1
#define REGISTER_FAILURE_MSG_TYPE 2
#define LOC_REQUEST_MSG_TYPE 3
#define LOC_SUCCESS_MSG_TYPE 4
#define LOC_FAILURE_MSG_TYPE 5
#define EXECUTE_MSG_TYPE 6
#define EXECUTE_SUCCESS_MSG_TYPE 7
#define EXECUTE_FAILURE_MSG_TYPE 8
#define TERMINATE_MSG_TYPE 9

//Bonus functionality message types
#define LOCATION_CACHE_REQUEST_MESSAGE_TYPE 10
#define LOCATION_CACHE_REQUEST_SUCCESS_MESSAGE_TYPE 11
#define LOCATION_CACHE_REQUEST_FAIL_MESSAGE_TYPE 12


struct RegisterMessage {
	string hostname;
	int port;
	string name;
	int *argTypes;
	int argsLength;
	long functionAddress;
};

struct RegisterSuccessMessage {
	int reasonCode;
};

struct RegisterFailureMessage {
	int reasonCode;
};

struct LOC_RequestMessage {
	string name;
	int *argTypes;
	int argsLength;
};

struct LOC_SuccessMessage {
	string hostname;
	int port;
	long functionAddress;
};

struct LOC_FailureMessage {
	int reasonCode;
};

struct ExecuteMessage {
	string name;
	int *argTypes;
	void  **args;
	int argsLength;
	long functionAddress;
};

struct ExecuteSuccessMessage {
	string name;
	int *argTypes;
	void **args;
	int argsLength;
};

struct ExecuteFailureMessage {
	int reasonCode;
};

struct TerminateMessage {

};

struct ExecutionThreadArgData {
	int connectionID;
	char *data;
};

struct LOC_CacheRequestMessage {
	string name;
	int *argTypes;
	int argsLength;
};

struct LOC_CacheSuccessMessage {
	int responseSize;
	vector <string> hostnames;
	vector <int> ports;
	vector <long> functionAddresses;
};

struct LOC_CacheFailMessage {
	int reasonCode;
};

//function prototypes:

int calculateArgsTypesLength(int *argTypes);

int countArrayElements(int argType);

int findArgType(int argType);

int placeIntegerIntoCharArray(int startIndex, int value, char *array);

int placeCharIntoCharArray(int startIndex, char value, char *array);

int placeStringIntoCharArray(int startIndex, string value, char *array);

int extractIntFromCharArray(int startIndex, char *array, int *output);

int extractLongFromCharArray(int startIndex, char *array, long *output);

int extractCharFromCharArray(int startIndex, char *array, char *output);

int extractStringFromCharArray(int currentIndex, char *array, int stringLength, string *output);


char *marshallRegisterMessage(struct RegisterMessage message, int *dataLength);

RegisterMessage unmarshallRegisterMessage(char *data);

char *marshallRegisterSuccessMessage(RegisterSuccessMessage message, int *dataLength);

RegisterSuccessMessage unmarshallRegisterSuccessMessage(char *data);

char *marshallRegisterFailureMessage(RegisterFailureMessage message, int *dataLength);

RegisterFailureMessage unmarshallRegisterFailureMessage(char *data);

char *marshallLOC_RequestMessage(LOC_RequestMessage message, int *dataLength);

LOC_RequestMessage unmarshallLOC_RequestMessage(char *data);

char *marshallLOC_SuccessMessage(LOC_SuccessMessage message, int *dataLength);

LOC_SuccessMessage unmarshallLOC_SuccessMessage(char *data);

char *marshallLOC_FailureMessage(LOC_FailureMessage message, int *dataLength);

LOC_FailureMessage unmarshallLOC_FailureMessage(char *data);

char *marshallExecuteMessage(ExecuteMessage message, int *dataLength);

ExecuteMessage unmarshallExecuteMessage(char *data);

char *marshallExecuteSuccessMessage(ExecuteSuccessMessage message, int *dataLength);

ExecuteSuccessMessage unmarshallExecuteSuccessMessage(char *data);

char *marshallExecuteFailureMessage(ExecuteFailureMessage message, int *dataLength);

ExecuteFailureMessage unmarshallExecuteFailureMessage(char *data);

char *marshallTerminateMessage(TerminateMessage message, int *dataLength);

TerminateMessage unmarshallTerminateMessage(char *data);

char *marshallLOCCacheRequestMessage(LOC_CacheRequestMessage message, int* dataLength);

LOC_CacheRequestMessage unmarshallLOCCacheRequestMessage(char* data);

char *marshalLOCCacheSuccessMessage(LOC_CacheSuccessMessage message, int* dataLength);

LOC_CacheSuccessMessage unmarshallLOCCacheSuccessMessage(char* data);

char *marshalLOCCacheFailMessage(LOC_CacheFailMessage message, int* dataLength);

LOC_CacheFailMessage unmarshallLOCCacheFailMessage(char* data);

