#ifndef CACHE_INSERT_ENDPOINT_H
#define CACHE_INSERT_ENDPOINT_H

#include "cache_insert_policy.h"

class CacheInsertEndpoint : public CacheInsertPolicy {
public:

        CacheInsertEndpoint() :
        CacheInsertPolicy() {
        }

        void insert(OverlayID &key, HostAddress &value) {
                DLLNode *n;
                HostAddress ha;
                if (hm->lookup(key, &n) || rt->lookup(key, &ha) || key == myOID) {
//                        puts("cache->insert called");
//                        exit(45);

                        return;
                }
                OverlayID *oid = new OverlayID(key.GetOverlay_id(), key.GetPrefix_length(), key.MAX_LENGTH);
                HostAddress *host_address = new HostAddress(string(value.GetHostName().c_str()),
                        value.GetHostPort());
                DLLNode *new_node = new DLLNode(*oid, *host_address);
                dll->append2Head(new_node);
                hm->add(*oid, new_node);
                (*cache_size)++;
                //printf("cache->insert called non match, size = %d", *cache_size);
                //exit(45);
        }
};

#endif