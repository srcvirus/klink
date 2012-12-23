#ifndef UTIL_H_
#define UTIL_H_

#include <iostream>
#include <stdlib.h>
#include "../plnode/ds/lookup_table.h"
#include "../plnode/ds/lookup_table_iterator.h"
#include "../plnode/ds/overlay_id.h"
#include "../plnode/ds/host_address.h"

using namespace std;

//class OverlayID;
//class 

int getBitAtPosition(int value, int n) {
        return (((value & (1 << n)) >> n) & 0x00000001);
}

void printBits(int value, int length) {
        for (int i = length - 1; i >= 0; i--) {
                cout << getBitAtPosition(value, i);
        }
}

char* printBits2String(int value, int length) {
        char* result = new char[length];
        for (int i = length - 1; i >= 0; i--) {
                result[i] = '0' + getBitAtPosition(value, i);
        }
        return result;
}

char* printRoutingTable2String(LookupTable<OverlayID, HostAddress> &rtable) {
        int index = 0, size = 0;
        LookupTableIterator<OverlayID, HostAddress> rtable_iter(&rtable);
        rtable_iter.reset_iterator();
        while (rtable_iter.hasMoreKey()) {
                OverlayID oid = rtable_iter.getNextKey();
                HostAddress ha;
                rtable.lookup(oid, &ha);
                size += oid.getStringSize() + 5 + ha.getStringSize() + 5;
        }
        size += 1;
        char* result = new char[size];

        rtable_iter.reset_iterator();
        while (rtable_iter.hasMoreKey()) {
                OverlayID oid = rtable_iter.getNextKey();
                HostAddress ha;
                rtable.lookup(oid, &ha);
                sprintf((result + index), "%s --> %s<br/>", oid.toString(), ha.toString());
                index += oid.getStringSize() + 5 + ha.getStringSize() + 5;
        }
        result[size] = '\0';
        return result;
}

char* printIndexTable2String(LookupTable<string, HostAddress> &itable) {
        int index = 0, size = 0;
        LookupTableIterator<string, HostAddress> itable_iter(&itable);
        itable_iter.reset_iterator();
        while (itable_iter.hasMoreKey()) {
                string name = itable_iter.getNextKey();
                HostAddress ha;
                itable.lookup(name, &ha);
                size += name.size() + 5 + ha.getStringSize() + 5;
        }
        size += 1;
        char* result = new char[size];

        itable_iter.reset_iterator();
        while (itable_iter.hasMoreKey()) {
                string name = itable_iter.getNextKey();
                HostAddress ha;
                itable.lookup(name, &ha);
                sprintf((result + index), "%s --> %s<br/>", name.c_str(), ha.toString());
                index += name.size() + 5 + ha.getStringSize() + 5;
        }
        result[size] = '\0';
        return result;
}

int stringHash(string str) {
        return atoi(str.c_str());
}

unsigned GetNumberOfDigits(unsigned number) {
        int digits = 0;
        while (number != 0) {
                number /= 10;
                digits++;
        }
        return digits;
}

#endif
