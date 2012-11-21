#include "rm/common.h"
#include "rm/reedmuller.h"

using namespace std;

class ReedMuller {
    int r, m;
    reedmuller rm;
public:
    ReedMuller();
    ReedMuller(int r, int m);
    int* encode(int* message);
    int* decode(int* codeword);

    int getLenght(int *array) {
        return (sizeof (array) / sizeof *(array));
    }
};

ReedMuller::ReedMuller(int r, int m) {
    this->r = r;
    this->m = m;
    this->rm = reedmuller_init(r, m);
}

int* ReedMuller::encode(int* message) {
    if (getLenght(message) != rm->k) {
#ifdef DEBUG_MESSAGE
        cout << "ERROR in rm.cc: rm-k = " << rm->k << " but message length " << getLenght(message) << "."
#endif
                return 0;
    }
    int* codeword = new int[rm->n];
    reedmuller_encode(rm, message, codeword);
    return codeword;
}

int* ReedMuller::decode(int* codeword) {
    if (getLenght(codeword) != rm->n) {
#ifdef DEBUG_MESSAGE
        cout << "ERROR in rm.cc: rm-n = " << rm->n << " but codeword length " << getLenght(codeword) << "."
#endif
                return 0;
    }
    int* message = new int[rm->k];
    reedmuller_decode(rm, codeword, message);
    return message;
}

int main() {
    puts("asdsd");
    return 0;
}
