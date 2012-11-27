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

#endif	/* READMULLERCODE_H */