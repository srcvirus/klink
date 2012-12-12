#include <iostream>

using namespace std;

int getBitAtPosition(int value, int n) const {
    return (((value & (1 << n)) >> n) & 0x00000001);
}

void printBits(int value, int length) {
    for (int i = length - 1; i >= 0; i--) {
        cout << getBitAtPosition(value, i);
    }
}

