/*
 * CS 454 Assignment 3
 * July 13, 2012
 * Group Member 1: Evgeny Shulyatyev (ID 20242378)
 * Group Member 2: Shihabur Rahman Chowdhury (ID 20453766)
 * */

#ifndef DATABASE_CC
#define DATABASE_CC_

#include "database.h"
#include <string>
#include <cstring>
#include <stdio.h>

using namespace std;

int LocationDatabase::addLocation(string key, Location loc)
{
	int ret = 0;
	int serverIndex = -1;

	if(database.find(key) == database.end())
	{
		for(int i = 0; i < (int)servers.size(); i++)
		{
			if(servers[i] == loc.hostName)
			{
				serverIndex = i;
				break;
			}
		}
		if(serverIndex < 0)
		{
			servers.push_back(loc.hostName);
			serverIndex = (int)servers.size() - 1;
		}
		loc.serverIndex = serverIndex;
		vector <Location> vLoc;
		vLoc.push_back(loc);
		database.insert(make_pair(key, vLoc));
	}
	else
	{
		bool duplicate = false;
		vector <Location>& vLoc = database[key];
		for(int i = 0; i < vLoc.size(); i++)
		{
			if(vLoc[i].hostName == loc.hostName)
			{
				vLoc[i].port = loc.port;
				vLoc[i].skeletonAddress = loc.skeletonAddress;
				duplicate = true;
				ret = 0;
				break;
			}
		}

		if(!duplicate)
		{
			for(int i = 0; i < servers.size(); i++)
			{
				if(servers[i] == loc.hostName)
				{
					serverIndex = i;
					break;
				}
			}
			if(serverIndex < 0)
			{
				servers.push_back(loc.hostName);
				serverIndex = (int)servers.size() - 1;
			}
			loc.serverIndex = serverIndex;
			database[key].push_back(loc);
		}
	}

	return ret;
}

Location LocationDatabase::getLocation(string key)
{
	Location ret;
	ret.hostName = "";
	ret.port = -1;
	ret.skeletonAddress = -1;
	int i;
	if(database.find(key) != database.end())
	{
		vector <Location>& locations = database[key];
		int i = 0;
		while(true)
		{
			for(i = 0; i < locations.size(); i++)
			{
				if(locations[i].serverIndex == nextLocation)
				{
					ret = database[key][i];
					nextLocation = (nextLocation + 1) % (int)servers.size();
					break;
				}
			}

			if(i >= locations.size())
				nextLocation = (nextLocation + 1) % (int)servers.size();
			else break;

		}
	}

	return ret;
}

vector <Location> LocationDatabase::getAllLocations(string key)
{
	vector <Location> ret;
	ret.clear();
	if(database.find(key) != database.end())
		ret = database[key];

	return ret;
}

void LocationDatabase::removeLocation(int connectionId)
{
	map < string, vector <Location> >::iterator it;
	int i;
	string key;
	vector <string> eraseList;

	for(it = database.begin(); it != database.end(); it++)
	{
		key = (*it).first;
		vector <Location>& vLoc = (*it).second;
		for(i = 0; i < (int)vLoc.size(); i++)
		{
			if(vLoc[i].connectionIdentifier == connectionId)
				break;
		}
		if(i < (int)vLoc.size())
		{
			vLoc.erase(vLoc.begin() + i);
			if(vLoc.empty()) eraseList.push_back(key);
		}
	}
	for(i = 0; i < (int)eraseList.size(); i++)
		database.erase(eraseList[i]);
}
void LocationDatabase::removeLocation(string key)
{
	database.erase(key);
}

bool isArray(int argType)
{
	int mask = 0x0000FFFF;

	if(argType & mask)
		return true;

	return false;
}

string toHexString(int value)
{
	string hexString;
	char* buf;
	int stringLength = (sizeof(int) << 1) + 1;
	buf = new char[stringLength];
	sprintf(buf,"%x",value);
	int padSize = (stringLength - 1) - strlen(buf);
	for(int i = 0; i < padSize; i++) hexString += "0";
	hexString += buf;
	delete[] buf;
	return hexString;
}

string hash(const char* name, int* argTypes, int argLength)
{
	string hashValue = name;
	string hexString = "";
	int nameLength = strlen(name);
	int mask = 0x0000FFFF;
	int temp;
	int i;
	for(i = 0; i < argLength; i++, argTypes++)
	{
		temp = *argTypes;
		if(isArray(temp))
		{
			temp |= mask;
		}
		hexString = toHexString(temp);
		hashValue += hexString;
	}

	return hashValue;
}
#endif
