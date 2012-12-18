
#include "cache_insert_policy.h"

class CacheInsertEndpoint : public CacheInsertPolicy {
public:

        CacheInsertEndpoint() : CacheInsertPolicy() {
        }

        void insert(OverlayID *key, HostAddress *value) {
                DLLNode *n;
                HostAddress ha;
                if (hm->lookup(*key, &n) || rt->lookup(*key, &ha) || (*key) == myOID) {
                        return;
                }
                DLLNode *node = new DLLNode(key, value);
                dll->append2Head(node);
                hm->add(*key, node);
        }
};
