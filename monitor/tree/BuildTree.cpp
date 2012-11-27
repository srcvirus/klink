#include<iostream>

#include "BuildTree.h"

using namespace std;

BuildTree::BuildTree(int treeSize) {
    this->treeSize = treeSize;
    this->idArray = new OverlayID[treeSize];
    this->rtArray = new RoutingTable[treeSize];
    this->max_height = ceil(log2(treeSize));
}

int BuildTree::GetHeight(int index) {
    if (index < pow(2, max_height) - treeSize)
        return max_height - 1;
    return max_height;
}

int BuildTree::GetIndexOfLongestMatchedPrefix(OverlayID id) {
    int maxLength = 0, length;
    int maxIndex = 0;
    for (int i = 0; i < treeSize; i++) {
        length = id.GetMatchedPrefixLength(idArray[i]);
        if (length > maxLength) {
            maxLength = length;
            maxIndex = i;
        }
    }
    return maxIndex;
}

void BuildTree::execute() {
    ReedMuller *rm = new ReedMuller(2, 4);
    for (int i = 0; i < this->treeSize; i++) {
        idArray[i] = OverlayID(rm->array2int(rm->encode(rm->int2array(i, rm->rm->k)), rm->rm->n), GetHeight(i), rm->rm->n);
    }

    int nbrIndex;
    for (int i = 0; i < this->treeSize; i++) {
        for (int j = 0; j < GetHeight(i); j++) {
            OverlayID nbrPattern = (idArray + i)->ToggleBitAtPosition(rm->rm->n - j);
            nbrIndex = GetIndexOfLongestMatchedPrefix(nbrPattern);
            rtArray[i].add(idArray[nbrIndex], IPAddress(1, 2, 3, 4));
        }
    }
}

int main() {
    puts("here here");
    BuildTree bt = BuildTree(9);
    bt.execute();

    return 0;
}
