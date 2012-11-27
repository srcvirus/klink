#include "lookup_table.h"
#include "../peer/OverlayID.h"
#include "ip_address.h"

class RoutingTable : public LookupTable<OverlayID, IPAddress> {
public:
    RoutingTable();
    ~RoutingTable();
    
    bool add(OverlayID key, IPAddress value);
};