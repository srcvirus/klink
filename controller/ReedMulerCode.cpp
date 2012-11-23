#include "ReadMullerCode.h"

#include<iostream>

using namespace std;

//template<typename T, std::size_t N> inline 
//std::size_t size(T(&)[N]){return N;}


int main() {
//    ReedMuller rm = ReedMuller(3,6);
//    cout << "message length = " << rm.rm->k << endl;
//    cout << "codeword length = " << rm.rm->n << endl;
//    int message[] = {1,0,1,0,0,0,1,0,1,0,0,0,1,1,0,0};
//    int *codeword = rm.encode(message);
//    for(int i=0;i<rm.rm->n;i++)
//        cout << codeword[i];
//    cout << endl;
    
    
    struct intbits{
        int lsb: 1;
        int m: 30;
        int msb: 1;
    };
    
    union flex_int{
        intbits bits; 
        int value;
    } test;
    
    int a = 0;
    cout << sizeof a << endl;
    cout << sizeof test << endl;
    
    test.value = 0;
    cout << test.value << endl;
    test.bits.m = 1;
    cout << test.value << endl;
    
    
    return 0; 
}
