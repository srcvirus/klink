/*
 * dictionary.h
 *
 *  Created on: 2012-11-21
 *      Author: sr2chowd
 */

#ifndef DICTIONARY_H_
#define DICTIONARY_H_

#include <map>
#include <vector>
#include <stdio.h>

using namespace std;

template <class KeyType, class ValueType>
class LookupTable
{
	map <KeyType, ValueType> table;

public:

	//typedef typename::vector <KeyType> vk;

	LookupTable(){ table.clear(); }
	virtual ~LookupTable(){ table.clear(); }

	bool add(KeyType key, ValueType value);
	bool lookup(const KeyType& key, ValueType* value);
	bool update(const KeyType& key, ValueType value);
	bool remove(const KeyType& key);
	vector <KeyType> getKeySet()
	{
		typename::vector <KeyType> keySet;

		typename map <KeyType, ValueType>::iterator mapIt;

		for(mapIt = table.begin(); mapIt != table.end(); mapIt++)
			keySet.push_back((*mapIt).first);

		return keySet;
	}
};

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

#endif /* DICTIONARY_H_ */
