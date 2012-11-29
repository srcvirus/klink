#include "double_linked_list.h"
#include "ip_address.h"
#include "overlay_id.h"
#include "cache.h"
#include "cache_insert_endpoint.h"
#include "cache_replace_LRU.h"

int main() {
    CacheInsertEndpoint *cinEnd = new CacheInsertEndpoint();
    CacheReplaceLRU *crLRU = new CacheReplaceLRU();
    Cache *c = new Cache(cinEnd, crLRU, 5);
    OverlayID *id1 = new OverlayID(6, 1, 1);
    c->add(new OverlayID(1, 1, 1), new IPAddress());
    c->add(new OverlayID(2, 1, 1), new IPAddress());
    c->add(new OverlayID(3, 1, 1), new IPAddress());
    c->add(new OverlayID(4, 1, 1), new IPAddress());
    c->add(new OverlayID(5, 1, 1), new IPAddress());
    c->add(id1, new IPAddress());
    c->add(new OverlayID(7, 1, 1), new IPAddress());
    c->add(new OverlayID(8, 1, 1), new IPAddress());
    c->lookup(new OverlayID(6, 1, 1));
    c->print();
}