#include "rm/reedmuller.h"
#include "rm/common.h"


class rm{
	int r, m;
	reedmuller rm;
public:
	rm(int r, int m);
	int* encode(int* message);
	int* decode(int* codeword);
};

rm::rm(int r, int m){
	rm = readMuller_init(r, m);
}

int* rm::encode(int* message){
	if(strlen(message) != rm->k){
		#ifdef DEBUG_MESSAGE
		cout << "ERROR in rm.cc: rm-k = " << rm->k << " but message length " << strlen(message) << "."
		#endif
		return NULL;
	}
	int* codeword = new int[rm->n];
	reedmuller_encode(rm, message, codeword);
	return codeword;
}

int* rm::decode(int* codeword){
	if(strlen(codeword) != rm->n){
		#ifdef DEBUG_MESSAGE
		cout << "ERROR in rm.cc: rm-n = " << rm->n << " but codeword length " << strlen(codeword) << "."
		#endif
		return NULL;
	}
	int* message = new int[rm->k];
	reedmuller_decode(rm, codeword, message);
	return message;
}

int main(){
	cout << "asdsd";
	return 0;
}
