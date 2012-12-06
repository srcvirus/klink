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
	typename map <KeyType, ValueType>::iterator table_iterator;
public:

	//typedef typename::vector <KeyType> vk;

	LookupTable(){ table.clear(); }
	virtual ~LookupTable(){ table.clear(); table_iterator = table.begin(); }
	int size(){ return table.size(); }
	void clear(){ table.clear(); }

	bool add(KeyType key, ValueType value);
	bool lookup(KeyType key, ValueType* value);
	bool update(KeyType key, ValueType value);
	bool remove(KeyType key);
	vector <KeyType> getKeySet()
	{
		typename::vector <KeyType> keySet;

		typename map <KeyType, ValueType>::iterator mapIt;

		for(mapIt = table.begin(); mapIt != table.end(); mapIt++)
			keySet.push_back((*mapIt).first);

		return keySet;
	}
	void reset_iterator(){ table_iterator = table.begin(); }
	KeyType getNextKey()
	{
		KeyType ret = (*table_iterator).first;
		table_iterator++;
		return ret;
	}
	bool hasMoreKey()
	{
		return table_iterator != table.end();
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
bool LookupTable<KeyType, ValueType>::lookup(KeyType key, ValueType* value)
{
	if(table.find(key) == table.end())
	{
		value = NULL;
		return false;
	}
	else
	{
		*value = (*table.find(key)).second;
		return true;
	}
}

template <typename KeyType, typename ValueType>
bool LookupTable<KeyType, ValueType>::update(KeyType key, ValueType value)
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
bool LookupTable<KeyType, ValueType>::remove(KeyType key)
{
	if(table.find(key) != table.end())
	{
		table.erase(key);
		return true;
	}
	return false;
}

#endif /* DICTIONARY_H_ */
