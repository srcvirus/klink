/* 
 * File:   ReadMuller.h
 * Author: mfbari
 *
 * Created on November 22, 2012, 5:37 PM
 */

#ifndef READMULLERCODE_H
#define	READMULLERCODE_H

#include "common.h"
#include "reedmuller.h"

#include<iostream>

using namespace std;

//template<typename T, std::size_t N> inline 
//std::size_t size(T(&)[N]){return N;}

class ReedMuller {
    int r, m;
public:    
    reedmuller rm;
    ReedMuller();
    ReedMuller(int r, int m);
    int* encode(int* message);
    int* decode(int* codeword);
    unsigned int array2int(int* array, int size);
    int* int2array(unsigned int value, int size);
};

ReedMuller::ReedMuller(int r, int m) {
    this->r = r;
    this->m = m;
    this->rm = reedmuller_init(r, m);
}

int* ReedMuller::encode(int* message) {
//    if (size(message) != rm->k) {
//#ifdef DEBUG_MESSAGE
//        cout << "ERROR in rm.cc: rm-k = " << rm->k << " but message length " << getLenght(message) << "."
//#endif
//                return 0;
//    }
    int* codeword = new int[rm->n];
    reedmuller_encode(rm, message, codeword);
    return codeword;
}

int* ReedMuller::decode(int* codeword) {
//    if (size(codeword) != rm->n) {
//#ifdef DEBUG_MESSAGE
//        cout << "ERROR in rm.cc: rm-n = " << rm->n << " but codeword length " << getLenght(codeword) << "."
//#endif
//                return 0;
//    }
    int* message = new int[rm->k];
    reedmuller_decode(rm, codeword, message);
    return message;
}

unsigned int ReedMuller::array2int(int* array, int size){
    unsigned int value = 0;
    for (int i = 0; i < size; i++) {
        value |= (array[i] & 0x00000001) << (size - i - 1);
    }
    return value;

}

int* ReedMuller::int2array(unsigned int value, int size){
    int *array = new int[size];
    for (int i = 0; i < size; i++) {
        array[i] = (((value & (1 << i)) >> i) & 0x00000001);
    }
    return array;
}
#endif	/* READMULLERCODE_H */