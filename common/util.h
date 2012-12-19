#ifndef UTIL_H_
#define UTIL_H_

#include <iostream>
#include <stdlib.h>

using namespace std;

int getBitAtPosition(int value, int n)
{
	return (((value & (1 << n)) >> n) & 0x00000001);
}

void printBits(int value, int length)
{
	for (int i = length - 1; i >= 0; i--)
	{
		cout << getBitAtPosition(value, i);
	}
}

int stringHash(string str)
{
	return atoi(str.c_str());
}

#endif
