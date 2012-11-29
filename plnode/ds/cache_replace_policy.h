/* 
 * File:   cache_replace_policy.h
 * Author: mfbari
 *
 * Created on November 29, 2012, 10:11 AM
 */

#ifndef CACHE_REPLACE_POLICY_H
#define	CACHE_REPLACE_POLICY_H

#include "double_linked_list.h"
#include "ip_address.h"
#include "overlay_id.h"

class CacheReplacePolicy{
    DoublyLinkedList *ddl;
    LookupTable<OverlayID, IPAddress> *hm;
public:
    CacheReplacePolicy(DoublyLinkedList *ddl, LookupTable<OverlayID, IPAddress> *hm){
        this->ddl = ddl;
        this->hm = hm;
    }
    virtual bool evict() = 0;
};

#endif	/* CACHE_REPLACE_POLICY_H */

