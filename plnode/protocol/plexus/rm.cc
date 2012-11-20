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
	int* codeword = new int[rm->n];
	reedmuller_encode(rm, message, codeword);
	return codeword;
}

int* rm::decode(int* codeword){
	int* message = new int[rm->k];
	reedmuller_decode(rm, codeword, message);
	return message;
}

int main(){
	cout << "asdsd";
	return 0;
}
