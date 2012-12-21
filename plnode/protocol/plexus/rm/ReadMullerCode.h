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
        int r, m, k, n;
public:
        reedmuller rm;
        ReedMuller();
        ReedMuller(int r, int m);
        int* encode(int* message);
        int decode(int codeword);
        int* __decode(int* codeword);
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

int ReedMuller::decode(int codeword) {
        return array2int(__decode(int2array(codeword, rm->n)), rm->k);
}

int* ReedMuller::__decode(int* codeword) {
        //    if (size(codeword) != rm->n) {
        //#ifdef DEBUG_MESSAGE
        //        cout << "ERROR in rm.cc: rm-n = " << rm->n << " but codeword length " << getLenght(codeword) << "."
        //#endif
        //                return 0;
        //    }

        cout << "codeword: ";
        char ch;
        for (int i = 0; i < rm->n; i++) {
                ch = codeword[i] + '0';
                cout << ch;
        }
        cout << endl;
        
        int* message = new int[rm->k];
        reedmuller_decode(rm, codeword, message);
        return message;
}

unsigned int ReedMuller::array2int(int* array, int size) {
        unsigned int value = 0;
        for (int i = 0; i < size; i++) {
                value |= (array[i] & 0x00000001) << (size - i - 1); // i -> (size - i - 1)
        }
        return value;

}

int* ReedMuller::int2array(unsigned int value, int size) {
        int *array = new int[size];
        for (int i = 0; i < size; i++) {
                //array[i] = (((value & (1 << i)) >> i) & 0x00000001);
                int shift = size - i - 1;
                array[i] = (((value & (1 << shift)) >> shift) & 0x00000001);
        }
        return array;
}
//int main() {
////    ReedMuller rm = ReedMuller(3,6);
////    cout << "message length = " << rm.rm->k << endl;
////    cout << "codeword length = " << rm.rm->n << endl;
////    int message[] = {1,0,1,0,0,0,1,0,1,0,0,0,1,1,0,0};
////    int *codeword = rm.encode(message);
////    for(int i=0;i<rm.rm->n;i++)
////        cout << codeword[i];
////    cout << endl;
//
//
//    struct intbits{
//        int lsb: 1;
//        int m: 30;
//        int msb: 1;
//    };
//
//    union flex_int{
//        intbits bits;
//        int value;
//    } test;
//
//    int a = 0;
//    cout << sizeof a << endl;
//    cout << sizeof test << endl;
//
//    test.value = 0;
//    cout << test.value << endl;
//    test.bits.m = 1;
//    cout << test.value << endl;
//
//
//    return 0;
//}

#endif	/* READMULLERCODE_H */
