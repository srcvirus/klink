#include <cmath>

#include "../../plnode/protocol/plexus/rm/ReadMullerCode.h"
#include "../../plnode/ds/routing_table.h"

class BuildTree {
public:
    int treeSize;
    OverlayID *idArray;
    RoutingTable *rtArray;
    int max_height;
public:
    BuildTree(int treeSize);
    int GetHeight(int index);
    int GetIndexOfLongestMatchedPrefix(OverlayID id);
    void execute();
};