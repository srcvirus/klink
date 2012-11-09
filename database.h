/*
 * CS 454 Assignment 3
 * July 13, 2012
 * Group Member 1: Evgeny Shulyatyev (ID 20242378)
 * Group Member 2: Shihabur Rahman Chowdhury (ID 20453766)
 * */

#ifndef BINDER_UTIL_H_
#define DATABASE_H_

#include <string>
#include <vector>
#include <utility>
#include <map>

using namespace std;

// Data Structures for Location Database

typedef struct Location
{
	string hostName;
	int serverIndex;
	int port;
	int connectionIdentifier;
	long skeletonAddress;
	Location(){;}
	Location(string host, int port, long skeletonAddress):hostName(host), port(port), skeletonAddress(skeletonAddress){;}
};

typedef struct LocationDatabase
{
	map < string, vector < Location > > database;
	vector <string> servers;
	int nextLocation;

	LocationDatabase(){ nextLocation = 0; database.clear(); servers.clear(); }
	int addLocation(string key, Location loc);
	Location getLocation(string key);
	vector <Location> getAllLocations(string key);
	void removeLocation(string key);
	void removeLocation(int connectionId);
	void clear()
	{
		database.clear();
		servers.clear();
		nextLocation = 0;
	}
} LocationDatabase;

//Hash and utility functions for mapping a function to a key in the location Database

string hash(const char* name, int* argTypes, int argLength);
string toHexString(int value);
bool isArray(int argType);

#endif /* BINDER_UTIL_H_ */
