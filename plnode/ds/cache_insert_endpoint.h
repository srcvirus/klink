
#include "cache_insert_policy.h"
#include "routing_table.h"

class CacheInsertEndpoint : public CacheInsertPolicy {
public:

    CacheInsertEndpoint() : CacheInsertPolicy() {
    }

    void insert(OverlayID *key, HostAddress *value) {
        DLLNode *node = new DLLNode(key, value);
        dll->append2Head(node);
        hm->add(*key, node);
    }
};
