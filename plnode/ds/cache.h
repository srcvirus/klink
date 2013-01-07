/* 
 * File:   Cache.h
 * Author: mfbari
 *
 * Created on November 29, 2012, 10:06 AM
 */

#ifndef CACHE_H
#define	CACHE_H

#include <iostream>
#include <cstdlib>
using namespace std;

#include "double_linked_list.h"
#include "lookup_table.h"
#include "cache_insert_policy.h"
#include "cache_replace_policy.h"
#include "overlay_id.h"
#include "host_address.h"

class Cache
{
public:
	DoublyLinkedList *dll;
	LookupTable<OverlayID, DLLNode*> *hm;
	CacheInsertPolicy *cinPolicy;
	CacheReplacePolicy *crPolicy;
	int capacity;
	int size;
	DLLNode *current;

//	Cache()
//	{
//		dll = new DoublyLinkedList();
//		hm = new LookupTable<OverlayID, DLLNode*>();
//		size = 0;
//		current = dll->getHead();
//	}

	Cache(CacheInsertPolicy *cinPolicy, CacheReplacePolicy *crPolicy,
			LookupTable<OverlayID, HostAddress>* rt, OverlayID oid,
			int capacity)
	{
		dll = new DoublyLinkedList();
		hm = new LookupTable<OverlayID, DLLNode*>();
		this->cinPolicy = cinPolicy;
		this->cinPolicy->setup(dll, hm, rt, oid, &size);
		this->crPolicy = crPolicy;
		this->crPolicy->setup(dll, hm);
		this->capacity = capacity;
		size = 0;
		current = dll->getHead();
	}

	void reset_iterator()
	{
		current = dll->getHead();
	}

	bool has_next()
	{
		if (current == NULL)
			return false;

		return current->next != NULL;
	}

	DLLNode* get_next()
	{
		if (current == NULL)
			return NULL;

		current = current->next;
		return current->prev;
	}

	void add(OverlayID &key, HostAddress &value)
	{
		if (size == capacity && !crPolicy->processHit(key))
		{
			crPolicy->evict();
                        size--;
		}
		cinPolicy->insert(key, value);
                puts("*******************************************************");
                printf("Cache size %d\n", size);
                puts(toString());
                puts("*******************************************************");
	}

	bool lookup(OverlayID key, HostAddress &hostAddress)
	{
		DLLNode *node = new DLLNode();
		if (hm->lookup(key, &node))
		{
			crPolicy->processHit(key);
			hostAddress = node->value;
			return true;
		}
		return false;
	}

	void print()
	{
		cout << size << endl;
		dll->printNodesForward();
	}
        
        int getStringSize(){
                return dll->getStringSize();
        }
        
        char* toString(){
                return dll->toString();
        }

        void setSize(int size) {
                this->size = size;
        }

        int getSize() const {
                return size;
        }
};

#endif	/* CACHE_H */

