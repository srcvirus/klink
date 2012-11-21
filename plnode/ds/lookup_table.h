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
	LookupTable();
	virtual ~LookupTable();

	bool add(KeyType key, ValueType value);
	bool lookup(KeyType key, ValueType* value);
	bool update(KeyType key, ValueType value);
	void remove(KeyType key);
	vector <KeyType> getKeySet();
};


#endif /* DICTIONARY_H_ */
