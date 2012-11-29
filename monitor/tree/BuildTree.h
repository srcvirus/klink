#include <cmath>

#include "../../plnode/protocol/plexus/rm/ReadMullerCode.h"
#include "../../plnode/ds/lookup_table.h"
#include "../../plnode/ds/overlay_id.h"
#include "../../plnode/ds/ip_address.h"

class BuildTree {
public:
    int treeSize;
    OverlayID *idArray;
    LookupTable<OverlayID, IPAddress> *rtArray;
    int max_height;
    ReedMuller *rm;
public:
    BuildTree(int treeSize);
    int GetHeight(int index);
    int GetIndexOfLongestMatchedPrefix(OverlayID id);
    void execute();
    void print();
};