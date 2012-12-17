#include <cmath>
#include <cstring>
#include <fstream>
#include <iostream>

#include "../../plnode/protocol/plexus/rm/ReadMullerCode.h"
#include "../../plnode/ds/lookup_table.h"
#include "../../plnode/ds/lookup_table_iterator.h"
#include "../../plnode/ds/overlay_id.h"
#include "../../plnode/ds/host_address.h"
#include "../../plnode/ds/GlobalData.h"

class BuildTree {
private:
    string fileName;
    int treeSize;
    OverlayID *idArray;
    HostAddress* hAddArray;
    LookupTable<OverlayID, HostAddress> *rtArray;
    LookupTable<OverlayID, HostAddress> *hosts;
    int max_height;
    ReedMuller *rm;
public:
    BuildTree(string fileName);
    int GetHeight(int index);
    int GetIndexOfLongestMatchedPrefix(OverlayID id);

    LookupTable<OverlayID, HostAddress>& getRoutingTablePtr(int index) {
        return rtArray[index];
    }

    OverlayID& getOverlayID(int index) {
        return idArray[index];
    }

    int getTreeSize() const {
        return treeSize;
    }

    HostAddress getHostAddress(int index) {
        return hAddArray[index];
    }
    void execute();
    void print();
    
    int GetBitAtPosition(int value, int n) const {
        return (((value & (1 << n)) >> n) & 0x00000001);
    }

    void printBits(int value, int length) {
        for (int i = length - 1; i >= 0; i--) {
            cout << GetBitAtPosition(value, i);
        }
    }
    
};

BuildTree::BuildTree(string fileName) {
    this->fileName = fileName;
}

int BuildTree::GetHeight(int index) {
    if (index < (pow(2.0, max_height) - treeSize))
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
    //open host list file
    ifstream hostListFile(fileName.c_str());

    if (hostListFile.is_open()) {
        // Initialize local variables
        hostListFile >> this->treeSize;
        this->idArray = new OverlayID[treeSize];
        this->hAddArray = new HostAddress[treeSize];
        this->rtArray = new LookupTable<OverlayID, HostAddress>[treeSize];
        this->hosts = new LookupTable<OverlayID, HostAddress>[treeSize];
        this->max_height = ceil(log2(treeSize));

        //rm code
        rm = GlobalData::rm;
        //cout << "k = " << rm->rm->k << endl;
        //cout << "n = " << rm->rm->n << endl;

        //read file and generate overlayid, hostname and port data
        string hostName;
        int hostPort;
        //the following variable are required for method2 of overlay id generation
        int pattern = 0;
        int nodesAtMaxHeight = 2 * treeSize - pow(2.0, max_height);
        int nodesAtPrevLevel = treeSize - nodesAtMaxHeight;
        int startingNodeAtPrevLevel = pow(2.0, max_height - 1) - nodesAtPrevLevel;
        int st = startingNodeAtPrevLevel;
        for (int i = 0; i < treeSize; i++) {
            //hostname and port
            hostListFile >> hostName;
            hostListFile >> hostPort;
            HostAddress ha;
            ha.SetHostName(hostName);
            ha.SetHostPort(hostPort);
            hAddArray[i] = ha;

            //generate overlayids
            //method 1: not in use
            //idArray[i] = OverlayID(rm->array2int(rm->encode(rm->int2array(i, rm->rm->k)), rm->rm->n), GetHeight(i), rm->rm->n);
            //method 2: complicated but makes forwarding easier
            pattern = (((i < nodesAtMaxHeight) ? i : st++) <<
                    (rm->rm->k - ((i < nodesAtMaxHeight) ? max_height : (max_height - 1))));
            //cout << "Pattern = ";
            //printBits(pattern, rm->rm->k);
            //cout << " ";
            //idArray[i] = OverlayID(rm->array2int(rm->encode(rm->int2array(pattern, rm->rm->k)), rm->rm->n), (i < nodesAtMaxHeight) ? max_height : (max_height - 1), rm->rm->n);
            idArray[i] = OverlayID(pattern, (i < nodesAtMaxHeight) ? max_height : (max_height - 1));
            //idArray[i].printBits();
            //cout << " pl = " << idArray[i].GetPrefix_length() << endl;

            //save in map for later retrieval
            hosts->add(idArray[i], hAddArray[i]);
        }
        hostListFile.close();

        //build routing table
        int nbrIndex;
        cout << "OID MAX LENGHT " << OverlayID::MAX_LENGTH << endl;
        for (int i = 0; i < this->treeSize; i++) {
            rtArray[i] = LookupTable<OverlayID, HostAddress > ();
            //toggle each bit (upto prefix length) and find the nbr
            OverlayID replica = idArray[i];
            for (int j = 0; j < idArray[i].GetPrefix_length(); j++) {
                OverlayID nbrPattern = idArray[i].ToggleBitAtPosition(OverlayID::MAX_LENGTH - j - 1);
                replica = replica.ToggleBitAtPosition(OverlayID::MAX_LENGTH - j - 1);
                nbrIndex = GetIndexOfLongestMatchedPrefix(nbrPattern);
                if (idArray[nbrIndex] != idArray[i]) {
                    HostAddress ha;
                    hosts->lookup(idArray[nbrIndex], &ha);
                    rtArray[i].add(idArray[nbrIndex], ha);
                }
            }
            //            cout << "CW = ";
            //            idArray[i].printBits();
            //            cout << " pl = " << idArray[i].GetPrefix_length() << " replica = ";
            //            replica.printBits();
            //            cout << endl;
            //add link to replica :: toggled all bits (replica)
            nbrIndex = GetIndexOfLongestMatchedPrefix(replica);
            if (idArray[nbrIndex] != idArray[i]) {
                HostAddress ha;
                hosts->lookup(idArray[nbrIndex], &ha);
                //rtArray[i].add(idArray[nbrIndex], ha);
            }
        }
    } else
        cout << "ERROR: opening host list file.";
}

void BuildTree::print() {
    cout << "RM: k = " << rm->rm->k << " n = " << rm->rm->n << endl;
    for (int i = 0; i < treeSize; i++) {
        cout << "CW = ";
        idArray[i].printBits();
        cout << " pl = " << idArray[i].GetPrefix_length();
        cout << endl << "RT" << endl;

        LookupTableIterator <OverlayID, HostAddress> rtable_itr(&rtArray[i]);
        rtable_itr.reset_iterator();
        while(rtable_itr.hasMoreKey())
        {
        	OverlayID key = rtable_itr.getNextKey();
        	key.printBits();
        	cout << endl;
        }

        /*vector<OverlayID> keys = rtArray[i].getKeySet();
        for (int j = 0; j < keys.size(); j++) {
            keys[j].printBits();
            cout << endl;
        }*/
        cout << "==================================" << endl;
    }
}

