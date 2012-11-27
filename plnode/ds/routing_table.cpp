#include "routing_table.h"


using namespace std;

RoutingTable::RoutingTable() : LookupTable<OverlayID, IPAddress>(){
}

bool RoutingTable::add(OverlayID key, IPAddress value){
    return LookupTable<OverlayID, IPAddress>::add(key, value);
}

RoutingTable::~RoutingTable(){
}
