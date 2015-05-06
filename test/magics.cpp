#define _TEST 1

#include <algorithm>
#include <iostream>
#include <vector>
#include "assert.h"
#include "../src/inlines.h"
#include "../src/squares.h"
#include "../src/magics.cpp"
#include "../src/cboard.cpp"

using namespace std;

// Tests generateSubsets with a basic example.
int testGenerateSubsets() {
    cout << "Testing generateSubsets" << endl;

    BB b = exp_2(5) | exp_2(45) | exp_2(63);
    vector<BB> subsets;
    vector<BB> expectedSubsets = {
        0,
        exp_2(5),
        exp_2(45),
        exp_2(63),
        exp_2(5) | exp_2(45),
        exp_2(5) | exp_2(63),
        exp_2(45) | exp_2(63),
        b,
    };
    magics::generateSubsets(b, &subsets);
    sort(begin(subsets), end(subsets));
    sort(begin(expectedSubsets), end(expectedSubsets));
    ASSERT(subsets == expectedSubsets, "Wrong subsets");

    return 1;
}

// Very simple test case for bishop magics
int testBishopMagics() {
    cout << "Testing bishop magics" << endl;

    CBoard board("1b6/8/8/4P3/8/8/8/8", "b", "", "-", "0", "0");
    BB expectedMoves = exp_2(A7) | exp_2(C7) | exp_2(D6) | exp_2(E5);
    magics::populateBishopTable(B8);
    BB moves = magics::bishopMoves(B8, board.occupied);
    ASSERT(moves == expectedMoves, "Bishop magic test failed");

    return 1;
}

// Very simple test case for rook magics
int testRookMagics() {
    cout << "Testing bishop magics" << endl;

    CBoard board("R7/P7/8/8/8/8/8/8", "b", "", "-", "0", "0");
    BB expectedMoves = exp_2(A7);
    for(ind square = B8; square >= H8; square--) {
        expectedMoves |= exp_2(square);
    }
    magics::populateRookTable(A8);
    BB hashKey = magics::hashRook(board.occupied & ~exp_2(A8), A8);
    hashKey = magics::hashRook(36028797018963968, A8);
    BB moves = magics::ROOK_MOVES[A8][hashKey];
    ASSERT(moves == expectedMoves, "Rook magic test failed");

    return 1;
}

int main() {
    int t = 0;

    t += testGenerateSubsets();
    t += testBishopMagics();
    t += testRookMagics();

    cout << endl;
    cout << t << " test(s) OK" << endl;
    return 0;
}

