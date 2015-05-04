#define _TEST 1

#include <algorithm>
#include "assert.h"
#include "../src/cboard.cpp"

using namespace std;

// Test pawnGen: non-capture pawn move generation
int testPawnGen() {
    cout << "Testing pawnGen..." << endl;

    mv moveList[256] = { 0 };
    mv * moves = moveList;

    CBoard board("rnb1kb1r/1p3p1p/p3pp2/4p3/3N1P2/q1N5/P1PQ2PP/1R2KB1R", "w", "Kkq", "-", "0", "12");
    board.pawnGen(&moves);
    mv expectedMoves[256] = { 0 };
    expectedMoves[0] = move::make(H2, H3, PAWN, NONE, NONE, NONE, NONE);
    expectedMoves[1] = move::make(H2, H4, PAWN, NONE, NONE, NONE, DOUBLE_PAWN_MOVE_W);
    expectedMoves[2] = move::make(G2, G3, PAWN, NONE, NONE, NONE, NONE);
    expectedMoves[3] = move::make(G2, G4, PAWN, NONE, NONE, NONE, DOUBLE_PAWN_MOVE_W);
    expectedMoves[4] = move::make(F4, F5, PAWN, NONE, NONE, NONE, NONE);
    sort(begin(moveList), end(moveList));
    sort(begin(expectedMoves), end(expectedMoves));
    ASSERT(!memcmp(moveList, expectedMoves, sizeof(moveList[0]) * 256), "Incorrect pawn move generation");

    return 1;
}

int main() {
    int t = 0;

    t += testPawnGen();

    cout << endl;
    cout << t << " test(s) OK" << endl;
    return 0;
}
