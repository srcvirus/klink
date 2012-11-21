#include "rm/common.h"
#include "rm/reedmuller.h"

using namespace std;

class ReedMuller{
	int r, m;
	reedmuller rm;
public:
	ReedMuller();
	ReedMuller(int r, int m);
	int* encode(int* message);
	int* decode(int* codeword);
};

ReedMuller::ReedMuller(int r, int m)
{
	this->r = r;
	this->m = m;
	this->rm = reedmuller_init(r, m);
}

int* ReedMuller::encode(int* message){
	if(strlen(message) != ReedMuller->k){
		#ifdef DEBUG_MESSAGE
		cout << "ERROR in rm.cc: rm-k = " << ReedMuller->k << " but message length " << strlen(message) << "."
		#endif
		return NULL;
	}
	int* codeword = new int[ReedMuller->n];
	reedmuller_encode(rm, message, codeword);
	return codeword;
}

int* ReedMuller::decode(int* codeword){
	if(strlen(codeword) != ReedMuller->n){
		#ifdef DEBUG_MESSAGE
		cout << "ERROR in rm.cc: rm-n = " << ReedMuller->n << " but codeword length " << strlen(codeword) << "."
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
