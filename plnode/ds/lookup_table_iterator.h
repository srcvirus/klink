/*
 * lookup_table_iterator.h
 *
 *  Created on: 2012-12-14
 *      Author: sr2chowd
 */

#include "lookup_table.h"
#include <pthread.h>
#include <map>

template <class KeyType, class ValueType>
class LookupTableIterator
{
	LookupTable <KeyType, ValueType>* table_ptr;
	typename map <KeyType, ValueType>::iterator table_iterator;
	pthread_rwlock_t iterator_read_lock;

public:

	LookupTableIterator(){ table_ptr = NULL; }
	LookupTableIterator(LookupTable <KeyType, ValueType>* ptr){ table_ptr = ptr; }

	void reset_iterator()
	{
		table_iterator = table_ptr->begin();
	}

	KeyType getNextKey()
	{
		KeyType ret = (*table_iterator).first;
		table_iterator++;
		return ret;
	}

	bool hasMoreKey()
	{
		return table_iterator != table_ptr->end();
	}
};


