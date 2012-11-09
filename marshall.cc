/*
 * CS 454 Assignment 3
 * July 13, 2012
 * Group Member 1: Evgeny Shulyatyev (ID 20242378)
 * Group Member 2: Shihabur Rahman Chowdhury (ID 20453766)
 * */


#ifndef UTIL_C
#define UTIL_C

#include <stdio.h>
#include <string>
#include "rpc.h"
#include "marshall.h"


using namespace std;

int calculateArgsTypesLength(int *argTypes)
{
	//this is excluding the trailing 0
	int length = 0;

	for(int *currentPos = argTypes; *currentPos != 0; currentPos++, length++);

	return length;
}

int placeIntegerIntoCharArray(int startIndex, int value, char *array)
{
	int i = 0;

	for(i = startIndex; i < sizeof(int) + startIndex; i++)
	{
		//cast LSByte to char
		char byte = (char) value;
		//place char byte into the outMessage
		array[i] = byte;
		//now bit shift by 1 byte = 8 bits
		value >>= 8;
	}

	return i;
}

int placeLongIntoCharArray(int startIndex, long value, char *array)
{
	int i = 0;

	for(i = startIndex; i < sizeof(long) + startIndex; i++)
	{
		//cast LSByte to char
		char byte = (char) value;
		//place char byte into the outMessage
		array[i] = byte;
		//now bit shift by 1 byte = 8 bits
		value >>= 8;
	}

	return i;
}

int placeCharIntoCharArray(int startIndex, char value, char *array)
{
	array[startIndex] = value;
	return startIndex+1;
}

int placeStringIntoCharArray(int startIndex, string value, char *array)
{
	int i = 0;

	for(i = startIndex; i < value.size() + startIndex; i++)
		array[i] = value[i-startIndex];

	return i;
}

int extractLongFromCharArray(int startIndex, char *array, long *output)
{
	int i = 0;
	*output = 0;

	for(i = (startIndex + sizeof(long) - 1); i >= startIndex; i--)
	{
		//cast MSByte to char
		char byte = array[i];
		long temp = byte & 0xFF;
		//place char byte into the outMessage
		//now bit shift by 1 byte = 8 bits
		*output <<= 8;
		//now add
		*output += temp;
	}

	return startIndex+sizeof(long);
}

int extractIntFromCharArray(int startIndex, char *array, int *output)
{
	int i = 0;
	*output = 0;

	for(i = (startIndex + sizeof(int) - 1); i >= startIndex; i--)
	{
		//cast MSByte to char
		char byte = array[i];
		int temp = byte & 0x000000FF;
		//place char byte into the outMessage
		//now bit shift by 1 byte = 8 bits
		*output <<= 8;
		//now add
		*output += temp;
	}

	return startIndex+sizeof(int);
}

int extractCharFromCharArray(int startIndex, char *array, char *output)
{
	*output = array[startIndex];
	return startIndex+1;
}

int extractStringFromCharArray(int currentIndex, char *array, int stringLength, string *output)
{
	for(int i = 0; i < stringLength; i++)
	{
		char ch = 0;
		currentIndex = extractCharFromCharArray(currentIndex, array, &ch);
		*output += ch;
	}

	return currentIndex;
}

char *marshallRegisterMessage(struct RegisterMessage message, int *dataLength)
{
	//int ipAddress, int port, string name, int *argTypes

	int currentIndex = 0;
	char *outData;
	int argsLength = calculateArgsTypesLength(message.argTypes);
	int length = sizeof(int) + sizeof(char) + sizeof(int) + message.hostname.size() + sizeof(int) + sizeof(int) + message.name.size() + sizeof(int) + (sizeof(int) * argsLength) + sizeof(long);

	*dataLength = length;

	outData = new char[length];

	//first, pack the length
	currentIndex = placeIntegerIntoCharArray(currentIndex, length, outData);

	//next, pack the type
	currentIndex = placeCharIntoCharArray(currentIndex, REGISTER_MSG_TYPE, outData);

	//pack the length of the host name
	currentIndex = placeIntegerIntoCharArray(currentIndex, message.hostname.size(), outData);

	//now pack the host name itself:

	currentIndex = placeStringIntoCharArray(currentIndex, message.hostname, outData);

	//pack the port
	currentIndex = placeIntegerIntoCharArray(currentIndex, message.port, outData);

	//pack the length of the name
	currentIndex = placeIntegerIntoCharArray(currentIndex, message.name.size(), outData);

	//now pack the name itself
	currentIndex = placeStringIntoCharArray(currentIndex, message.name, outData);

	//put the number of argTypes:
	currentIndex = placeIntegerIntoCharArray(currentIndex, argsLength, outData);

	//pack the argTypes (everything except the last 0):
	for(int *currentPos = message.argTypes; *currentPos!=0; currentPos++)
		currentIndex = placeIntegerIntoCharArray(currentIndex, *currentPos, outData);

	//now pack the pointer to the function address:
	currentIndex = placeLongIntoCharArray(currentIndex, message.functionAddress, outData);

	return outData;
}

RegisterMessage unmarshallRegisterMessage(char *data)
{
	struct RegisterMessage output;
	int nameLength = 0;
	int currentIndex = 0;
	int argsLength = 0;

	//first, get the length of the host name
	currentIndex = extractIntFromCharArray(currentIndex, data, &nameLength);

	//now get the name out
	currentIndex = extractStringFromCharArray(currentIndex, data, nameLength, &(output.hostname));

	//next, get the port number
	currentIndex = extractIntFromCharArray(currentIndex, data, &(output.port));

	//next, get the length of the name
	currentIndex = extractIntFromCharArray(currentIndex, data, &nameLength);

	//now get the name out
	currentIndex = extractStringFromCharArray(currentIndex, data, nameLength, &(output.name));

	//get the length of argsType:
	currentIndex = extractIntFromCharArray(currentIndex, data, &argsLength);

	output.argTypes = new int[argsLength];

	//now get the argTypes:
	for(int i = 0; i < argsLength; i++)
	{
		int arg = 0;
		currentIndex = extractIntFromCharArray(currentIndex, data, &arg);
		output.argTypes[i] = arg;
	}

	//lastly, get the function address:
	currentIndex = extractLongFromCharArray(currentIndex, data, &output.functionAddress);

	output.argsLength = argsLength;

	return output;
}

char *marshallRegisterSuccessMessage(RegisterSuccessMessage message, int *dataLength)
{
	int currentIndex = 0;
	char *outData;
	int length = sizeof(int) + sizeof(char) + sizeof(int);

	*dataLength = length;

	//allocate memory to outData
	outData = new char[length];

	//first, pack the length
	currentIndex = placeIntegerIntoCharArray(currentIndex, length, outData);
	//next, pack the type
	currentIndex = placeCharIntoCharArray(currentIndex, REGISTER_SUCCESS_MSG_TYPE, outData);

	//place the reason code:
	currentIndex = placeIntegerIntoCharArray(currentIndex, message.reasonCode, outData);

	return outData;
}

RegisterSuccessMessage unmarshallRegisterSuccessMessage(char *data)
{
	struct RegisterSuccessMessage output;
	int currentIndex = 0;

	//get the reason code
	currentIndex = extractIntFromCharArray(currentIndex, data, &output.reasonCode);

	return output;
}

char *marshallRegisterFailureMessage(RegisterFailureMessage message, int *dataLength)
{
	int currentIndex = 0;
	char *outData;
	int length = sizeof(int) + sizeof(char) + sizeof(int);

	*dataLength = length;

	//allocate memory to outData
	outData = new char[length];

	//first, pack the length
	currentIndex = placeIntegerIntoCharArray(currentIndex, length, outData);
	//next, pack the type
	currentIndex = placeCharIntoCharArray(currentIndex, REGISTER_FAILURE_MSG_TYPE, outData);

	//place the reason code:
	currentIndex = placeIntegerIntoCharArray(currentIndex, message.reasonCode, outData);

	return outData;
}

RegisterFailureMessage unmarshallRegisterFailureMessage(char *data)
{
	struct RegisterFailureMessage output;
	int currentIndex = 0;

	//get the reason code
	currentIndex = extractIntFromCharArray(currentIndex, data, &output.reasonCode);

	return output;
}


char *marshallLOC_RequestMessage(LOC_RequestMessage message, int *dataLength)
{
	int currentIndex = 0;
	char *outData;
	int argsLength = calculateArgsTypesLength(message.argTypes);
	int length = sizeof(int) + sizeof(char) + sizeof(int) + (sizeof(char) * message.name.size()) + sizeof(int) + (sizeof(int) * argsLength);

	*dataLength = length;

	//allocate memory to outData
	outData = new char[length];


	//first, pack the length
	currentIndex = placeIntegerIntoCharArray(currentIndex, length, outData);

	//next, pack the type
	currentIndex = placeCharIntoCharArray(currentIndex, LOC_REQUEST_MSG_TYPE, outData);

	//pack the length of the name
	currentIndex = placeIntegerIntoCharArray(currentIndex, message.name.size(), outData);

	//now pack the name itself
	currentIndex = placeStringIntoCharArray(currentIndex, message.name, outData);

	//put the number of argTypes:
	currentIndex = placeIntegerIntoCharArray(currentIndex, argsLength, outData);

	//pack the argTypes (everything except the last 0):
	for(int *currentPos = message.argTypes; *currentPos!=0; currentPos++)
		currentIndex = placeIntegerIntoCharArray(currentIndex, *currentPos, outData);

	return outData;

}

LOC_RequestMessage unmarshallLOC_RequestMessage(char *data)
{
	struct LOC_RequestMessage output;
	int nameLength = 0;
	int currentIndex = 0;
	int argsLength = 0;

	//first, get the length of the name:
	currentIndex = extractIntFromCharArray(currentIndex, data, &nameLength);
	//now get the name string itself:
	currentIndex = extractStringFromCharArray(currentIndex, data, nameLength, &(output.name));

	//get the length of argsType:
	currentIndex = extractIntFromCharArray(currentIndex, data, &argsLength);

	output.argTypes = new int[argsLength];

	//now get the argTypes:
	for(int i = 0; i < argsLength; i++)
	{
		int arg = 0;
		currentIndex = extractIntFromCharArray(currentIndex, data, &arg);
		output.argTypes[i] = arg;
	}

	output.argsLength = argsLength;

	return output;
}

char *marshallLOC_SuccessMessage(LOC_SuccessMessage message, int *dataLength)
{
	int currentIndex = 0;
	char *outData;
	int length = sizeof(int) + sizeof(char) + sizeof(int) + message.hostname.size() + sizeof(int) + sizeof(long);

	*dataLength = length;

	//allocate memory to outData
	outData = new char[length];

	//first, pack the length
	currentIndex = placeIntegerIntoCharArray(currentIndex, length, outData);
	//next, pack the type
	currentIndex = placeCharIntoCharArray(currentIndex, LOC_SUCCESS_MSG_TYPE, outData);

	//pack the length of the host name
	currentIndex = placeIntegerIntoCharArray(currentIndex, message.hostname.size(), outData);

	//now pack the hostname itself:
	currentIndex = placeStringIntoCharArray(currentIndex, message.hostname, outData);

	//pack the port
	currentIndex = placeIntegerIntoCharArray(currentIndex, message.port, outData);

	//pack the function address
	currentIndex = placeLongIntoCharArray(currentIndex, message.functionAddress, outData);

	return outData;
}

LOC_SuccessMessage unmarshallLOC_SuccessMessage(char *data)
{
	struct LOC_SuccessMessage output;
	int currentIndex = 0;
	int nameLength = 0;

	//first, get the length of the host name
	currentIndex = extractIntFromCharArray(currentIndex, data, &nameLength);

	//now get the host name itself:
	currentIndex = extractStringFromCharArray(currentIndex, data, nameLength, &(output.hostname));

	//next, get the port number
	currentIndex = extractIntFromCharArray(currentIndex, data, &output.port);

	//get the function address:
	currentIndex = extractLongFromCharArray(currentIndex, data, &output.functionAddress);

	return output;
}

char *marshallLOC_FailureMessage(LOC_FailureMessage message, int *dataLength)
{
	int currentIndex = 0;
	char *outData;
	int length = sizeof(int) + sizeof(char) + sizeof(int);

	*dataLength = length;

	//allocate memory to outData
	outData = new char[length];

	//first, pack the length
	currentIndex = placeIntegerIntoCharArray(currentIndex, length, outData);
	//next, pack the type
	currentIndex = placeCharIntoCharArray(currentIndex, LOC_FAILURE_MSG_TYPE, outData);

	//place the reason code:
	currentIndex = placeIntegerIntoCharArray(currentIndex, message.reasonCode, outData);

	return outData;
}

LOC_FailureMessage unmarshallLOC_FailureMessage(char *data)
{
	struct LOC_FailureMessage output;
	int currentIndex = 0;

	//get the reason code
	currentIndex = extractIntFromCharArray(currentIndex, data, &output.reasonCode);

	return output;
}

int countArrayElements(int argType)
{
	int arrayLength = argType & 0xFFFF;

	if(arrayLength == 0)
		arrayLength = 1;

	return arrayLength;
}

int findArgType(int argType)
{
	int type = (argType & 0x00FF0000) >> 16;
	return type;
}

int calculateArgumentSize(int *argTypes, int argsLength)
{
	//argsLength is exlcluding trailing 0 (the actual length)
	int totalSize = 0;

	for(int i = 0; i<argsLength; i++)
	{
		int argumentSize = 0;
		//determine if the arg is an array or not
		int arrayLength = countArrayElements(argTypes[i]);

		//find out its type
		int type = findArgType(argTypes[i]);

		switch(type)
		{
			case ARG_CHAR:
				argumentSize = sizeof(char) * arrayLength;
				break;
			case ARG_SHORT:
				argumentSize = sizeof(short) * arrayLength;
				break;
			case ARG_INT:
				argumentSize = sizeof(int) * arrayLength;
				break;
			case ARG_LONG:
				argumentSize = sizeof(long) * arrayLength;
				break;
			case ARG_DOUBLE:
				argumentSize = sizeof(double) * arrayLength;
				break;
			case ARG_FLOAT:
				argumentSize = sizeof(float) * arrayLength;
				break;
		}

		totalSize += argumentSize;
	}

	return totalSize;
}


int placeShortIntoCharArray(int startIndex, short value, char *array)
{
	int i = 0;

	for(i = startIndex; i < sizeof(short) + startIndex; i++)
	{
		//cast LSByte to char
		char byte = (char) value;
		//place char byte into the outMessage
		array[i] = byte;
		//now bit shift by 1 byte = 8 bits
		value >>= 8;
	}

	return i;
}

int extractShortFromCharArray(int startIndex, char *array, short *output)
{
	int i = 0;
	*output = 0;

	for(i = (startIndex + sizeof(short) - 1); i >= startIndex; i--)
	{
		//cast MSByte to char
		char byte = array[i];
		short temp = byte & 0x000000FF;
		//place char byte into the outMessage
		//now bit shift by 1 byte = 8 bits
		*output <<= 8;
		//now add
		*output += temp;
	}

	return startIndex+sizeof(short);

}

int placeDoubleIntoCharArray(int startIndex, double value, char *array)
{
    int i;

    /*
    PLEASE NOTE: the following conversation was adapted from a solution posted on Stack Overflow.
    Reference:
    Stack Overflow. "Serialize Double and Float with C." Stack Overflow. Stack Overflow, n.d. Web. 7 July 2012. <http://stackoverflow.com/questions/3418319/serialize-double-and-float-with-c>.
    */

    union
    {
        double         d;
        unsigned char  bytes[sizeof(double)];
    } u;

    u.d = value;
    for (i=startIndex; i<startIndex + sizeof(double); ++i)
        array[i] = u.bytes[i-startIndex];

    return i;
}

int extractDoubleFromCharArray(int startIndex, char *array, double *output)
{
    int i = 0;

    /*
    PLEASE NOTE: the following conversation was adapted from a solution posted on Stack Overflow.
    Reference:
    Stack Overflow. "Serialize Double and Float with C." Stack Overflow. Stack Overflow, n.d. Web. 7 July 2012. <http://stackoverflow.com/questions/3418319/serialize-double-and-float-with-c>.
    */

    union
    {
        double         d;
        unsigned char  bytes[sizeof(double)];
    } u;

    //u.d = value;

    for (i=startIndex; i<startIndex + sizeof(double); ++i)
    	u.bytes[i-startIndex] = array[i];

    *output = u.d;

    return i;
}


int placeFloatIntoCharArray(int startIndex, float value, char *array)
{
    int i;

    /*
    PLEASE NOTE: the following conversation was adapted from a solution posted on Stack Overflow.
    Reference:
    Stack Overflow. "Serialize Double and Float with C." Stack Overflow. Stack Overflow, n.d. Web. 7 July 2012. <http://stackoverflow.com/questions/3418319/serialize-double-and-float-with-c>.
    */

    union
    {
        float         d;
        unsigned char  bytes[sizeof(float)];
    } u;

    u.d = value;
    for (i=startIndex; i<startIndex + sizeof(float); ++i)
        array[i] = u.bytes[i-startIndex];

    return i;
}

int extractFloatFromCharArray(int startIndex, char *array, float *output)
{
    int i = 0;

    /*
    PLEASE NOTE: the following conversation was adapted from a solution posted on Stack Overflow.
    Reference:
    Stack Overflow. "Serialize Double and Float with C." Stack Overflow. Stack Overflow, n.d. Web. 7 July 2012. <http://stackoverflow.com/questions/3418319/serialize-double-and-float-with-c>.
    */

    union
    {
        float         d;
        unsigned char  bytes[sizeof(float)];
    } u;

    //u.d = value;

    for (i=startIndex; i<startIndex + sizeof(float); ++i)
    	u.bytes[i-startIndex] = array[i];

    *output = u.d;

    return i;
}


int placeArgumentsIntoCharArray(int currentIndex, void **args, int *argTypes, int argsLength, char *outData)
{
	for(int i = 0; i < argsLength; i++)
	{
		void *currentArg = args[i];
		int arrayCount = countArrayElements(argTypes[i]);
		int type = findArgType(argTypes[i]);

		switch(type)
		{
			case ARG_CHAR:
				for(int j = 0; j < arrayCount; j++)
					currentIndex = placeCharIntoCharArray(currentIndex, (char) *(( (char *) currentArg) + j), outData);
				break;
			case ARG_SHORT:
				for(int j = 0; j < arrayCount; j++)
					currentIndex = placeShortIntoCharArray(currentIndex, (short) *(( (short *) currentArg) + j), outData);
				break;
			case ARG_INT:
				for(int j = 0; j < arrayCount; j++)
					currentIndex = placeIntegerIntoCharArray(currentIndex, (int) *(( (int *) currentArg) + j), outData);
				break;
			case ARG_LONG:
				for(int j = 0; j < arrayCount; j++)
					currentIndex = placeLongIntoCharArray(currentIndex, (long) *(( (long *) currentArg) + j), outData);
				break;
			case ARG_DOUBLE:
				for(int j = 0; j < arrayCount; j++)
					currentIndex = placeDoubleIntoCharArray(currentIndex, (double) *(( (double *) currentArg) + j), outData);
				break;
			case ARG_FLOAT:
				for(int j = 0; j < arrayCount; j++)
					currentIndex = placeFloatIntoCharArray(currentIndex, (float) *(( (float *) currentArg) + j), outData);
				break;
		}
	}

	return currentIndex;
}

int extractArgumentsFromCharArray(int currentIndex, int *argTypes, int argsLength, char *array, void **output)
{

	for(int i = 0; i < argsLength; i++)
	{
		int type = findArgType(argTypes[i]);
		int arrayCount = countArrayElements(argTypes[i]);

		switch(type)
		{
			case ARG_CHAR:
				output[i] = new char[arrayCount];
				for(int j = 0; j < arrayCount; j++)
					currentIndex = extractCharFromCharArray(currentIndex, array, ((char *) output[i]) + j);
				break;
			case ARG_SHORT:
				output[i] = new short[arrayCount];
				for(int j = 0; j < arrayCount; j++)
					currentIndex = extractShortFromCharArray(currentIndex, array, ((short *) output[i]) + j);
				break;
			case ARG_INT:
				output[i] = new int[arrayCount];
				for(int j = 0; j < arrayCount; j++)
					currentIndex = extractIntFromCharArray(currentIndex, array, ((int *) output[i]) + j);
				break;
			case ARG_LONG:
				output[i] = new long[arrayCount];
				for(int j = 0; j < arrayCount; j++)
					currentIndex = extractLongFromCharArray(currentIndex, array, ((long *) output[i]) + j);
				break;
			case ARG_DOUBLE:
				output[i] = new double[arrayCount];
				for(int j = 0; j < arrayCount; j++)
					currentIndex = extractDoubleFromCharArray(currentIndex, array, ((double *) output[i]) + j);
				break;
			case ARG_FLOAT:
				output[i] = new float[arrayCount];
				for(int j = 0; j < arrayCount; j++)
					currentIndex = extractFloatFromCharArray(currentIndex, array, ((float *) output[i]) + j);
				break;
		}
	}

	return currentIndex;
}

char *marshallExecuteMessage(ExecuteMessage message, int *dataLength)
{
	int currentIndex = 0;
	char *outData;

	int argsLength = calculateArgsTypesLength(message.argTypes);

	int argumentsMemSize = calculateArgumentSize(message.argTypes, argsLength);

	int length = sizeof(int) + sizeof(char) + sizeof(int) + message.name.size() + sizeof(int) + (sizeof(int) * argsLength) + argumentsMemSize + sizeof(long);

	*dataLength = length;

	outData = new char[length];

	//first, pack the length
	currentIndex = placeIntegerIntoCharArray(currentIndex, length, outData);

	//next, pack the type
	currentIndex = placeCharIntoCharArray(currentIndex, EXECUTE_MSG_TYPE, outData);

	//pack the length of the name
	currentIndex = placeIntegerIntoCharArray(currentIndex, message.name.size(), outData);

	//now pack the name itself
	currentIndex = placeStringIntoCharArray(currentIndex, message.name, outData);

	//put the number of argTypes:
	currentIndex = placeIntegerIntoCharArray(currentIndex, argsLength, outData);

	//pack the argTypes (everything except the last 0):
	for(int *currentPos = message.argTypes; *currentPos!=0; currentPos++)
		currentIndex = placeIntegerIntoCharArray(currentIndex, *currentPos, outData);

	//pack the args themselves:
	currentIndex = placeArgumentsIntoCharArray(currentIndex, message.args, message.argTypes, argsLength, outData);

	//now pack the function address:
	currentIndex = placeLongIntoCharArray(currentIndex, message.functionAddress, outData);

	return outData;
}

ExecuteMessage unmarshallExecuteMessage(char *data)
{
	struct ExecuteMessage output;
	int nameLength = 0;
	int currentIndex = 0;
	int argsLength = 0;

	//get the length of the name
	currentIndex = extractIntFromCharArray(currentIndex, data, &nameLength);

	//now get the name out
	currentIndex = extractStringFromCharArray(currentIndex, data, nameLength, &(output.name));

	//get the length of argsType:
	currentIndex = extractIntFromCharArray(currentIndex, data, &argsLength);

	output.argTypes = new int[argsLength];
	output.argsLength = argsLength;

	//now get the argTypes:
	for(int i = 0; i < argsLength; i++)
	{
		int arg = 0;
		currentIndex = extractIntFromCharArray(currentIndex, data, &arg);
		output.argTypes[i] = arg;
	}

	//now get the args:
	output.args = new void *[argsLength];
	currentIndex = extractArgumentsFromCharArray(currentIndex, output.argTypes, argsLength, data, output.args);

	//get the function address:
	currentIndex = extractLongFromCharArray(currentIndex, data, &output.functionAddress);

	return output;
}

char *marshallExecuteSuccessMessage(ExecuteSuccessMessage message, int *dataLength)
{
	int currentIndex = 0;
	char *outData;
	int argsLength = message.argsLength;

	int argumentsMemSize = calculateArgumentSize(message.argTypes, argsLength);

	int length = sizeof(int) + sizeof(char) + sizeof(int) + message.name.size() + sizeof(int) + (sizeof(int) * argsLength) + argumentsMemSize;

	*dataLength = length;

	outData = new char[length];

	//first, pack the length
	currentIndex = placeIntegerIntoCharArray(currentIndex, length, outData);

	//next, pack the type
	currentIndex = placeCharIntoCharArray(currentIndex, EXECUTE_SUCCESS_MSG_TYPE, outData);

	//pack the length of the name
	currentIndex = placeIntegerIntoCharArray(currentIndex, message.name.size(), outData);

	//now pack the name itself
	currentIndex = placeStringIntoCharArray(currentIndex, message.name, outData);

	//put the number of argTypes:
	currentIndex = placeIntegerIntoCharArray(currentIndex, argsLength, outData);

	//pack the argTypes (everything except the last 0):
	for(int i = 0; i < message.argsLength; i++)
		currentIndex = placeIntegerIntoCharArray(currentIndex, message.argTypes[i], outData);

	//pack the args themselves:
	currentIndex = placeArgumentsIntoCharArray(currentIndex, message.args, message.argTypes, argsLength, outData);


	return outData;
}

ExecuteSuccessMessage unmarshallExecuteSuccessMessage(char *data)
{
	struct ExecuteSuccessMessage output;
	int nameLength = 0;
	int currentIndex = 0;
	int argsLength = 0;

	//get the length of the name
	currentIndex = extractIntFromCharArray(currentIndex, data, &nameLength);

	//now get the name out
	currentIndex = extractStringFromCharArray(currentIndex, data, nameLength, &(output.name));

	//get the length of argsType:
	currentIndex = extractIntFromCharArray(currentIndex, data, &argsLength);
	output.argsLength = argsLength;

	output.argTypes = new int[argsLength];

	//now get the argTypes:
	for(int i = 0; i < argsLength; i++)
	{
		int arg = 0;
		currentIndex = extractIntFromCharArray(currentIndex, data, &arg);
		output.argTypes[i] = arg;
	}

	//now get the args:
	output.args = new void *[argsLength];
	currentIndex = extractArgumentsFromCharArray(currentIndex, output.argTypes, argsLength, data, output.args);

	return output;
}

char *marshallExecuteFailureMessage(ExecuteFailureMessage message, int *dataLength)
{
	int currentIndex = 0;
	char *outData;
	int length = sizeof(int) + sizeof(char) + sizeof(int);

	*dataLength = length;

	//allocate memory to outData
	outData = new char[length];

	//first, pack the length
	currentIndex = placeIntegerIntoCharArray(currentIndex, length, outData);
	//next, pack the type
	currentIndex = placeCharIntoCharArray(currentIndex, EXECUTE_FAILURE_MSG_TYPE, outData);

	//place the reason code:
	currentIndex = placeIntegerIntoCharArray(currentIndex, message.reasonCode, outData);

	return outData;
}

ExecuteFailureMessage unmarshallExecuteFailureMessage(char *data)
{
	struct ExecuteFailureMessage output;
	int currentIndex = 0;
	//get the reason code
	currentIndex = extractIntFromCharArray(currentIndex, data, &output.reasonCode);
	return output;
}

char *marshallTerminateMessage(TerminateMessage message, int *dataLength)
{
	int currentIndex = 0;
	char *outData;
	int length = sizeof(int) + sizeof(char);

	*dataLength = length;

	//allocate memory to outData
	outData = new char[length];

	//first, pack the length
	currentIndex = placeIntegerIntoCharArray(currentIndex, length, outData);
	//next, pack the type
	currentIndex = placeCharIntoCharArray(currentIndex, TERMINATE_MSG_TYPE, outData);

	return outData;
}

TerminateMessage unmarshallTerminateMessage(char *data)
{
	struct TerminateMessage output;
	return output;
}

char *marshallLOCCacheRequestMessage(LOC_CacheRequestMessage message, int* dataLength)
{
	int currentIndex = 0;
	char *outData;
	int argsLength = calculateArgsTypesLength(message.argTypes);
	int length = sizeof(int) + sizeof(char) + sizeof(int) + (sizeof(char) * message.name.size()) + sizeof(int) + (sizeof(int) * argsLength);

	*dataLength = length;

	//allocate memory to outData
	outData = new char[length];

	//first, pack the length
	currentIndex = placeIntegerIntoCharArray(currentIndex, length, outData);

	//next, pack the type
	currentIndex = placeCharIntoCharArray(currentIndex, LOCATION_CACHE_REQUEST_MESSAGE_TYPE, outData);

	//pack the length of the name
	currentIndex = placeIntegerIntoCharArray(currentIndex, message.name.size(), outData);

	//now pack the name itself
	currentIndex = placeStringIntoCharArray(currentIndex, message.name, outData);

	//put the number of argTypes:
	currentIndex = placeIntegerIntoCharArray(currentIndex, argsLength, outData);

	//pack the argTypes (everything except the last 0):
	for(int *currentPos = message.argTypes; *currentPos!=0; currentPos++)
		currentIndex = placeIntegerIntoCharArray(currentIndex, *currentPos, outData);

	return outData;

}

LOC_CacheRequestMessage unmarshallLOCCacheRequestMessage(char *data)
{
	struct LOC_CacheRequestMessage output;
	int nameLength = 0;
	int currentIndex = 0;
	int argsLength = 0;

	//first, get the length of the name:
	currentIndex = extractIntFromCharArray(currentIndex, data, &nameLength);
	//now get the name string itself:
	currentIndex = extractStringFromCharArray(currentIndex, data, nameLength, &(output.name));

	//get the length of argsType:
	currentIndex = extractIntFromCharArray(currentIndex, data, &argsLength);

	output.argTypes = new int[argsLength];

	//now get the argTypes:
	for(int i = 0; i < argsLength; i++)
	{
		int arg = 0;
		currentIndex = extractIntFromCharArray(currentIndex, data, &arg);
		output.argTypes[i] = arg;
	}
	char *marshallLOCCacheRequestMessage(LOC_CacheRequestMessage message, int* dataLength);
	output.argsLength = argsLength;

	return output;
}

char *marshalLOCCacheSuccessMessage(LOC_CacheSuccessMessage message, int *dataLength)
{
	char *outData;
	int i;
	int totalNameLength = 0;
	for(i = 0; i < message.responseSize; i++) totalNameLength += message.hostnames[i].size();

	int length = sizeof(int) + sizeof(char) + sizeof(int) + sizeof(int) * message.hostnames.size() + totalNameLength + message.ports.size() * sizeof(int) + message.functionAddresses.size() * sizeof(long);
	outData = new char[length];
	*dataLength = length;
	int currentIndex = 0;

	//place the total message length
	currentIndex = placeIntegerIntoCharArray(currentIndex, length, outData);

	//place the message type
	currentIndex = placeCharIntoCharArray(currentIndex, LOCATION_CACHE_REQUEST_SUCCESS_MESSAGE_TYPE, outData);

	//place the number of responses
	currentIndex = placeIntegerIntoCharArray(currentIndex, message.responseSize, outData);

	//place the (name_length, name) pairs
	for(i = 0; i < message.responseSize; i++)
	{
		currentIndex = placeIntegerIntoCharArray(currentIndex, message.hostnames[i].size(), outData);
		currentIndex = placeStringIntoCharArray(currentIndex, message.hostnames[i], outData);
	}

	//place the port numbers
	for(i = 0; i < message.responseSize; i++)
	{
		currentIndex = placeIntegerIntoCharArray(currentIndex, message.ports[i], outData);
	}

	//place the function addresses
	for(i = 0; i < message.responseSize; i++)
	{
		currentIndex = placeLongIntoCharArray(currentIndex, message.functionAddresses[i], outData);
	}

	return outData;
}

LOC_CacheSuccessMessage unmarshallLOCCacheSuccessMessage(char* data)
{
	LOC_CacheSuccessMessage output;
	int currentIndex = 0, responseSize, nameLength;
	int i;

	//get the number of responses
	currentIndex = extractIntFromCharArray(currentIndex, data, &responseSize);
	output.responseSize = responseSize;

	//get the (name_length, hostname pairs)
	for(i = 0; i < responseSize; i++)
	{
		string hostname;
		currentIndex = extractIntFromCharArray(currentIndex, data, &nameLength);
		currentIndex = extractStringFromCharArray(currentIndex, data, nameLength, &hostname);
		output.hostnames.push_back(hostname);
	}

	//get the port numbers
	for(i = 0; i < responseSize; i++)
	{
		int port;
		currentIndex = extractIntFromCharArray(currentIndex, data, &port);
		output.ports.push_back(port);
	}

	//get the function addresses
	for(i = 0; i < responseSize; i++)
	{
		long functionAddress;
		currentIndex = extractLongFromCharArray(currentIndex, data, &functionAddress);
		output.functionAddresses.push_back(functionAddress);
	}

	return output;
}

char *marshalLOCCacheFailMessage(LOC_CacheFailMessage message, int *dataLength)
{
	int currentIndex = 0;
	char *outData;
	int length = sizeof(int) + sizeof(char) + sizeof(int);

	*dataLength = length;

	//allocate memory to outData
	outData = new char[length];

	//first, pack the length
	currentIndex = placeIntegerIntoCharArray(currentIndex, length, outData);
	//next, pack the type
	currentIndex = placeCharIntoCharArray(currentIndex, LOCATION_CACHE_REQUEST_FAIL_MESSAGE_TYPE, outData);

	//place the reason code:
	currentIndex = placeIntegerIntoCharArray(currentIndex, message.reasonCode, outData);

	return outData;
}

LOC_CacheFailMessage unmarshallLOCCacheFailMessage(char* data)
{
	struct LOC_CacheFailMessage output;
	int currentIndex = 0;

	//get the reason code
	currentIndex = extractIntFromCharArray(currentIndex, data, &output.reasonCode);

	return output;
}


#endif

