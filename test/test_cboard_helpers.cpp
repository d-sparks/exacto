#include <string.h>
#include <map>
#include "assert.h"
#include "../src/cboard.cpp"
#include "../src/bb.cpp"

using namespace std;

// Test squareToIndex by checking that it is the inverse of the array indexToSquare on the
// appropriate domain.
int testSquareToIndex() {
    cout << "Testing squareToIndex..." << endl;
    for(ind i = 0; i < totalSquares; i++) {
        ASSERT(i == squareToIndex(indexToSquare[i]), "squareToIndex failed for i = " + to_string(i));
    }
    return 1;
}

int main() {
    int t = 0;

    t += testSquareToIndex();

    cout << endl;
    cout << t << " test(s) OK" << endl;
    return 0;
}
