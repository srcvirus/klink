/* 
 * File:   cache_insert_policy.h
 * Author: mfbari
 *
 * Created on November 29, 2012, 10:10 AM
 */

#ifndef CACHE_INSERT_POLICY_H
#define	CACHE_INSERT_POLICY_H

#include "overlay_id.h"
#include "host_address.h"
#include "double_linked_list.h"
#include "lookup_table.h"
#include "routing_table.h"

class CacheInsertPolicy{
protected:
    DoublyLinkedList *dll;
    LookupTable<OverlayID, DLLNode*> *hm;
    RoutingTable *rt;
public:

    CacheInsertPolicy(){
            
    }
    
    void setup(DoublyLinkedList *dll, LookupTable<OverlayID, DLLNode*> *hm, RoutingTable *rt){
        this->dll = dll;
        this->hm = hm;
        this->rt = rt;
    }
    
    virtual void insert(OverlayID *key, HostAddress *value) = 0;
};

#endif	/* CACHE_INSERT_POLICY_H */

