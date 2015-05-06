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
    expectedMoves[0] = moves::make(H2, H3, PAWN, NONE, NONE, NONE, NONE);
    expectedMoves[1] = moves::make(H2, H4, PAWN, NONE, NONE, NONE, DOUBLE_PAWN_MOVE_W);
    expectedMoves[2] = moves::make(G2, G3, PAWN, NONE, NONE, NONE, NONE);
    expectedMoves[3] = moves::make(G2, G4, PAWN, NONE, NONE, NONE, DOUBLE_PAWN_MOVE_W);
    expectedMoves[4] = moves::make(F4, F5, PAWN, NONE, NONE, NONE, NONE);
    sort(begin(moveList), end(moveList));
    sort(begin(expectedMoves), end(expectedMoves));
    ASSERT(!memcmp(moveList, expectedMoves, sizeof(moveList[0]) * 256), "Incorrect pawn move generation");

    return 1;
}

// Test pawnCaps: pawn capture moves
int testPawnCaps() {
    cout << "Testing pawnCaps..." << endl;

    mv moveList[256] = { 0 };
    mv * moves = moveList;

    CBoard board("rnb1kb1r/1p3p1p/p3pp2/4p3/3N1P2/q1N5/P1PQ2PP/1R2KB1R", "w", "Kkq", "-", "0", "12");
    board.pawnCaps(&moves);
    mv expectedMoves[256] = { 0 };
    expectedMoves[0] = moves::make(F4, E5, PAWN, PAWN, NONE, NONE, NONE);
    sort(begin(moveList), end(moveList));
    sort(begin(expectedMoves), end(expectedMoves));
    ASSERT(!memcmp(moveList, expectedMoves, sizeof(moveList[0]) * 256), "Incorrect pawn captures generation");

    return 1;
}

// Test knightGen: all knight moves
int testKnightGen() {
    cout << "Testing knightGen..." << endl;

    mv moveList[256] = { 0 };
    mv * moves = moveList;

    CBoard board("rnb1kb1r/1p3p1p/p3pp2/4p3/3N1P2/q7/P1PQ2PP/NR2KB1R", "w", "Kkq", "-", "0", "12");
    board.knightGen(&moves, true);
    mv expectedMoves[256] = { 0 };
    expectedMoves[0] = moves::make(A1, B3, KNIGHT, NONE, NONE, NONE, NONE);
    expectedMoves[1] = moves::make(D4, B3, KNIGHT, NONE, NONE, NONE, NONE);
    expectedMoves[2] = moves::make(D4, B5, KNIGHT, NONE, NONE, NONE, NONE);
    expectedMoves[3] = moves::make(D4, C6, KNIGHT, NONE, NONE, NONE, NONE);
    expectedMoves[4] = moves::make(D4, E6, KNIGHT, PAWN, NONE, NONE, NONE);
    expectedMoves[5] = moves::make(D4, F5, KNIGHT, NONE, NONE, NONE, NONE);
    expectedMoves[6] = moves::make(D4, F3, KNIGHT, NONE, NONE, NONE, NONE);
    expectedMoves[7] = moves::make(D4, E2, KNIGHT, NONE, NONE, NONE, NONE);
    sort(begin(moveList), end(moveList));
    sort(begin(expectedMoves), end(expectedMoves));
    ASSERT(!memcmp(moveList, expectedMoves, sizeof(moveList[0]) * 256), "Incorrect knight move generation");

    return 1;
}

// Test knightCaps: knight capture moves only
int testKnightCaps() {
    cout << "Testing knightCaps..." << endl;

    mv capList[256] = { 0 };
    mv * caps = capList;

    CBoard board("rnb1kb1r/1p3p1p/p3pp2/4p3/3N1P2/q7/P1PQ2PP/NR2KB1R", "w", "Kkq", "-", "0", "12");
    board.knightGen(&caps, false);
    mv expectedCaps[256] = { 0 };
    expectedCaps[0] = moves::make(D4, E6, KNIGHT, PAWN, NONE, NONE, NONE);
    ASSERT(!memcmp(capList, expectedCaps, sizeof(capList[0]) * 256), "Incorrect knight capture generation");

    return 1;
}

int main() {
    int t = 0;

    t += testPawnGen();
    t += testPawnCaps();
    t += testKnightGen();
    t += testKnightCaps();

    cout << endl;
    cout << t << " test(s) OK" << endl;
    return 0;
}
