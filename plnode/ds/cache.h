/* 
 * File:   Cache.h
 * Author: mfbari
 *
 * Created on November 29, 2012, 10:06 AM
 */

#ifndef CACHE_H
#define	CACHE_H

#include <iostream>
using namespace std;

#include "double_linked_list.h"
#include "lookup_table.h"
#include "cache_insert_policy.h"
#include "cache_replace_policy.h"
#include "overlay_id.h"
#include "ip_address.h"

class Cache {
public:
    DoublyLinkedList *dll;
    LookupTable<OverlayID, DLLNode*> *hm;
    CacheInsertPolicy *cinPolicy;
    CacheReplacePolicy *crPolicy;
    int capacity;
    int size;

    Cache() {
    }

    Cache(CacheInsertPolicy *cinPolicy, CacheReplacePolicy *crPolicy, int capacity) {
        dll = new DoublyLinkedList();
        hm = new LookupTable<OverlayID, DLLNode*>();
        this->cinPolicy = cinPolicy;
        this->cinPolicy->setup(dll, hm);
        this->crPolicy = crPolicy;
        this->crPolicy->setup(dll, hm);
        this->capacity = capacity;
        size = 0;
    }

    void add(OverlayID *key, IPAddress *value) {
        crPolicy->processHit(key);
        if (size == capacity) {
            crPolicy->evict();
        } else {
            size++;
        }
        cinPolicy->insert(key, value);
    }
    
    IPAddress* lookup(OverlayID* key){
        DLLNode *node;
        if(hm->lookup(*key, &node)){
            crPolicy->processHit(key);
            return node->value;
        }
        return NULL;
    }

    void print() {
        cout << size << endl;
        dll->printNodesForward();
    }
};




#endif	/* CACHE_H */

