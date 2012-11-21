/*
 * lookup_table.cc
 *
 *  Created on: 2012-11-21
 *      Author: sr2chowd
 */
#include "lookup_table.h"

using namespace std;

template <typename KeyType, typename ValueType>
bool LookupTable<KeyType, ValueType>::add(KeyType key, ValueType value)
{
	if(table.find(key) != table.end())
		return false;
	table.insert(make_pair(key, value));
	return true;
}

template <typename KeyType, typename ValueType>
bool LookupTable<KeyType, ValueType>::lookup(const KeyType& key, ValueType* value)
{
	if(table.find(key) == table.end())
	{
		value = NULL;
		return false;
	}
	else
	{
		*value = table[key];
		return true;
	}
}

template <typename KeyType, typename ValueType>
bool LookupTable<KeyType, ValueType>::update(const KeyType& key, ValueType value)
{
	if(table.find(key) == table.end())
	{
		table.insert(make_pair(key, value));
		return false;
	}
	else
	{
		table[key] = value;
		return true;
	}
}

template <typename KeyType, typename ValueType>
bool LookupTable<KeyType, ValueType>::remove(const KeyType& key)
{
	if(table.find(key) != table.end())
	{
		table.erase(key);
		return true;
	}
	return false;
}

template <typename KeyType, typename ValueType>
vector <KeyType> LookupTable<KeyType, ValueType>::getKeySet()
{
	vector <KeyType> keySet;

	typename map <KeyType, ValueType>::iterator mapIt;

	for(mapIt = table.begin(); mapIt != table.end(); mapIt++)
		keySet.push_back(mapIt);

	return keySet;
}






