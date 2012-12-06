#include <cmath>
#include <cstring>
#include <fstream>

#include "../../plnode/protocol/plexus/rm/ReadMullerCode.h"
#include "../../plnode/ds/lookup_table.h"
#include "../../plnode/ds/overlay_id.h"
#include "../../plnode/ds/host_address.h"

class BuildTree {
public:
    string fileName;
    int treeSize;
    OverlayID *idArray;
    LookupTable<OverlayID, HostAddress> *rtArray;
    int max_height;
    ReedMuller *rm;
public:
    BuildTree(string fileName);
    int GetHeight(int index);
    int GetIndexOfLongestMatchedPrefix(OverlayID id);
    void execute();
    void print();
};

BuildTree::BuildTree(string fileName) {
    this->fileName = fileName;
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
    //INIT

    ifstream hostListFile(fileName.c_str());

    if (hostListFile.is_open()) {

        hostListFile >> this->treeSize;
        this->idArray = new OverlayID[treeSize];
        this->rtArray = new LookupTable<OverlayID, HostAddress>[treeSize];
        this->max_height = ceil(log2(treeSize));


        rm = new ReedMuller(1, 2);
        for (int i = 0; i < this->treeSize; i++) {
            idArray[i] = OverlayID(rm->array2int(rm->encode(rm->int2array(i, rm->rm->k)), rm->rm->n), GetHeight(i), rm->rm->n);
        }

        int nbrIndex;
        string hostName;
        int hostPort;
        for (int i = 0; i < this->treeSize; i++) {
            rtArray[i] = LookupTable<OverlayID, HostAddress> ();
            for (int j = 0; j < GetHeight(i); j++) {
                OverlayID nbrPattern = idArray[i].ToggleBitAtPosition(rm->rm->n - j - 1);
                nbrIndex = GetIndexOfLongestMatchedPrefix(nbrPattern);
                if (idArray[nbrIndex] != idArray[i]) {
                    hostListFile >> hostName;
                    hostListFile >> hostPort;
                    HostAddress ha;
                    ha.SetHostName(hostName);
                    ha.SetHostPort(hostPort);
                    rtArray[i].add(idArray[nbrIndex], ha);
                }
            }
        }
        hostListFile.close();
    } else
        cout << "ERROR: opening host list file.";
}

void BuildTree::print() {
    cout << "RM: k = " << rm->rm->k << " n = " << rm->rm->n << endl;
    for (int i = 0; i < treeSize; i++) {
        cout << "CW = ";
        idArray[i].printBits();
        cout << " pl = " << GetHeight(i);
        cout << endl << "RT" << endl;
        vector<OverlayID> keys = rtArray[i].getKeySet();
        for (int j = 0; j < keys.size(); j++) {
            keys[j].printBits();
            cout << endl;
        }
        cout << "==================================" << endl;
    }
}