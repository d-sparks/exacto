#define _TEST 1

#include <algorithm>
#include "assert.h"
#include "../src/cboard.cpp"
#include "../src/magics.cpp"

using namespace std;

// Test pawnGen: non-capture pawn move generation
int testPawnGen() {
    cout << "Testing pawnGen..." << endl;

    mv moveList[256] = { 0 };
    mv * moves = moveList;

    // +---+---+---+---+---+---+---+---+
    // | r | n | b |   |[k]| b |   | r |
    // +---+---+---+---+---+---+---+---+
    // |   | p |   |   |   | p |   | p |
    // +---+---+---+---+---+---+---+---+
    // | p |   |   |   | p | p |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   | p |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   | N |   | P |   |   |
    // +---+---+---+---+---+---+---+---+
    // | q |   | N |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // | P |   | P | Q |   |   | P | P |
    // +---+---+---+---+---+---+---+---+
    // |   | R |   |   |[K]| B |   | R |
    // +---+---+---+---+---+---+---+---+

    CBoard board("rnb1kb1r/1p3p1p/p3pp2/4p3/3N1P2/q1N5/P1PQ2PP/1R2KB1R", "w", "Kkq", "-", "0", "12");
    board.pawnGen(&moves, 0, true);
    mv expectedMoves[256] = { 0 };
    expectedMoves[0] = moves::make(H2, H3, PAWN, NONE, NONE, NONE, REGULAR_MOVE);
    expectedMoves[1] = moves::make(H2, H4, PAWN, NONE, NONE, NONE, DOUBLE_PAWN_MOVE_W);
    expectedMoves[2] = moves::make(G2, G3, PAWN, NONE, NONE, NONE, REGULAR_MOVE);
    expectedMoves[3] = moves::make(G2, G4, PAWN, NONE, NONE, NONE, DOUBLE_PAWN_MOVE_W);
    expectedMoves[4] = moves::make(F4, F5, PAWN, NONE, NONE, NONE, REGULAR_MOVE);
    expectedMoves[5] = moves::make(F4, E5, PAWN, PAWN, NONE, NONE, REGULAR_MOVE);
    sort(begin(moveList), end(moveList));
    sort(begin(expectedMoves), end(expectedMoves));
    ASSERT(!memcmp(moveList, expectedMoves, sizeof(moveList[0]) * 256), "Incorrect pawn move generation");

    return 1;
}

// Test pawnGen: pawn promotion
int testPawnGenPromotions() {
    cout << "Testing pawnGen for pawn promotion..." << endl;

    mv moveList[256] = { 0 };
    mv * moves = moveList;

    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   |   |   | p |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   |   | R |   |
    // +---+---+---+---+---+---+---+---+

    CBoard board("8/8/8/8/8/8/7p/6R1", "b", "", "-", "0", "0");
    board.pawnGen(&moves, 0, true);
    mv expectedMoves[256] = { 0 };
    for(ind special = PROMOTE_QUEEN; special <= PROMOTE_KNIGHT; special++) {
        expectedMoves[special] = moves::make(H2, H1, PAWN, NONE, NONE, NONE, special);
        expectedMoves[100 + special] = moves::make(H2, G1, PAWN, ROOK, NONE, NONE, special);
    }
    sort(begin(moveList), end(moveList));
    sort(begin(expectedMoves), end(expectedMoves));
    ASSERT(!memcmp(moveList, expectedMoves, sizeof(moveList[0]) * 256), "Incorrect pawn promotion generation");

    return 1;
}

// Test pawnCaps: pawn capture moves
int testPawnCaps() {
    cout << "Testing pawnCaps..." << endl;

    mv capList[256] = { 0 };
    mv * caps = capList;

    // +---+---+---+---+---+---+---+---+
    // | r | n | b |   |[k]| b |   | r |
    // +---+---+---+---+---+---+---+---+
    // |   | p |   |   |   | p |   | p |
    // +---+---+---+---+---+---+---+---+
    // | p |   |   |   | p | p |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   | p |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   | N |   | P |   |   |
    // +---+---+---+---+---+---+---+---+
    // | q |   | N |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // | P |   | P | Q |   |   | P | P |
    // +---+---+---+---+---+---+---+---+
    // |   | R |   |   |[K]| B |   | R |
    // +---+---+---+---+---+---+---+---+

    CBoard board("rnb1kb1r/1p3p1p/p3pp2/4p3/3N1P2/q1N5/P1PQ2PP/1R2KB1R", "w", "Kkq", "-", "0", "12");
    board.pawnCaps(&caps, 0);
    mv expectedCaps[256] = { 0 };
    expectedCaps[0] = moves::make(F4, E5, PAWN, PAWN, NONE, NONE, REGULAR_MOVE);
    sort(begin(capList), end(capList));
    sort(begin(expectedCaps), end(expectedCaps));
    ASSERT(!memcmp(capList, expectedCaps, sizeof(capList[0]) * 256), "Incorrect pawn captures generation");

    return 1;
}

// Test pawnCaps: pawn enPassant captures
int testPawnCapsEnPassant() {
    cout << "Testing pawnCaps for enPassant..." << endl;

    mv capList[256] = { 0 };
    mv * caps = capList;

    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   | : |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   | P | p |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |[K]|   |   |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+

    CBoard board("8/8/8/3Pp3/8/8/8/K7", "w", "", "E6", "0", "0");
    board.pawnCaps(&caps, 0);
    mv expectedCaps[256] = { 0 };
    expectedCaps[0] = moves::make(D5, E6, PAWN, PAWN, NONE, NONE, EN_PASSANT_CAP_W);
    sort(begin(capList), end(capList));
    sort(begin(expectedCaps), end(expectedCaps));
    ASSERT(!memcmp(capList, expectedCaps, sizeof(capList[0]) * 256), "Incorrect en passant pawn captures generation");

    return 1;
}

// Test pawnGenPinned
int testPawnGenPinned() {
    cout << "Testing pawnGenPinned..." << endl;

    mv moveList[256] = { 0 };
    mv * moves = moveList;

    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // | b |   |   |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   | p | P |   |   |   | r |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   |   |   | r |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   |   | P |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   |   |[K]|   |
    // +---+---+---+---+---+---+---+---+

    CBoard board("8/b7/8/1pP3r1/8/7r/6P1/6K1", "w", "", "B6", "0", "0");
    masks::generateOpposite();
    board.pawnGenPinned(&moves, exp_2(C5) | exp_2(G2), G1, true);
    mv expectedMoves[256] = { 0 };
    expectedMoves[0] = moves::make(C5, B6, PAWN, PAWN, NONE, NONE, EN_PASSANT_CAP_W);
    expectedMoves[1] = moves::make(G2, G3, PAWN, NONE, NONE, NONE, REGULAR_MOVE);
    expectedMoves[2] = moves::make(G2, G4, PAWN, NONE, NONE, NONE, DOUBLE_PAWN_MOVE_W);
    sort(begin(moveList), end(moveList));
    sort(begin(expectedMoves), end(expectedMoves));
    ASSERT(!memcmp(moveList, expectedMoves, sizeof(moveList[0]) * 256), "Incorrect pinned pawn move generation");

    return 1;
}

// Test knightGen: all knight moves
int testKnightGen() {
    cout << "Testing knightGen..." << endl;

    mv moveList[256] = { 0 };
    mv * moves = moveList;

    // +---+---+---+---+---+---+---+---+
    // | r | n | b |   |[k]| b |   | r |
    // +---+---+---+---+---+---+---+---+
    // |   | p |   |   |   | p |   | p |
    // +---+---+---+---+---+---+---+---+
    // | p |   |   |   | p | p |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   | p |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   | N |   | P |   |   |
    // +---+---+---+---+---+---+---+---+
    // | q |   |   |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // | P |   | P | Q |   |   | P | P |
    // +---+---+---+---+---+---+---+---+
    // | N | R |   |   |[K]| B |   | R |
    // +---+---+---+---+---+---+---+---+

    CBoard board("rnb1kb1r/1p3p1p/p3pp2/4p3/3N1P2/q7/P1PQ2PP/NR2KB1R", "w", "Kkq", "-", "0", "12");
    board.knightGen(&moves, 0, true);
    mv expectedMoves[256] = { 0 };
    expectedMoves[0] = moves::make(A1, B3, KNIGHT, NONE, NONE, NONE, REGULAR_MOVE);
    expectedMoves[1] = moves::make(D4, B3, KNIGHT, NONE, NONE, NONE, REGULAR_MOVE);
    expectedMoves[2] = moves::make(D4, B5, KNIGHT, NONE, NONE, NONE, REGULAR_MOVE);
    expectedMoves[3] = moves::make(D4, C6, KNIGHT, NONE, NONE, NONE, REGULAR_MOVE);
    expectedMoves[4] = moves::make(D4, E6, KNIGHT, PAWN, NONE, NONE, REGULAR_MOVE);
    expectedMoves[5] = moves::make(D4, F5, KNIGHT, NONE, NONE, NONE, REGULAR_MOVE);
    expectedMoves[6] = moves::make(D4, F3, KNIGHT, NONE, NONE, NONE, REGULAR_MOVE);
    expectedMoves[7] = moves::make(D4, E2, KNIGHT, NONE, NONE, NONE, REGULAR_MOVE);
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

    // +---+---+---+---+---+---+---+---+
    // | r | n | b |   |[k]| b |   | r |
    // +---+---+---+---+---+---+---+---+
    // |   | p |   |   |   | p |   | p |
    // +---+---+---+---+---+---+---+---+
    // | p |   |   |   | p | p |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   | p |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   | N |   | P |   |   |
    // +---+---+---+---+---+---+---+---+
    // | q |   |   |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // | P |   | P | Q |   |   | P | P |
    // +---+---+---+---+---+---+---+---+
    // | N | R |   |   |[K]| B |   | R |
    // +---+---+---+---+---+---+---+---+

    CBoard board("rnb1kb1r/1p3p1p/p3pp2/4p3/3N1P2/q7/P1PQ2PP/NR2KB1R", "w", "Kkq", "-", "0", "12");
    board.knightGen(&caps, 0, false);
    mv expectedCaps[256] = { 0 };
    expectedCaps[0] = moves::make(D4, E6, KNIGHT, PAWN, NONE, NONE, REGULAR_MOVE);
    ASSERT(!memcmp(capList, expectedCaps, sizeof(capList[0]) * 256), "Incorrect knight capture generation");

    return 1;
}

// Test bishopGen: all bishop moves
int testBishopGen() {
    cout << "Testing bishopGen with quiet moves..." << endl;

    mv moveList[256] = { 0 };
    mv * moves = moveList;

    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   | P |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   | p |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   | b |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   | p |   |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+

    CBoard board("8/8/2P5/5p2/4b3/3p4/8/8", "b", "", "-", "0", "0");
    magics::populateBishopTable(E4);
    board.bishopGen(&moves, 0, true);
    mv expectedMoves[256] = { 0 };
    expectedMoves[0] = moves::make(E4, D5, BISHOP, NONE, NONE, NONE, REGULAR_MOVE);
    expectedMoves[1] = moves::make(E4, C6, BISHOP, PAWN, NONE, NONE, REGULAR_MOVE);
    expectedMoves[2] = moves::make(E4, F3, BISHOP, NONE, NONE, NONE, REGULAR_MOVE);
    expectedMoves[3] = moves::make(E4, G2, BISHOP, NONE, NONE, NONE, REGULAR_MOVE);
    expectedMoves[4] = moves::make(E4, H1, BISHOP, NONE, NONE, NONE, REGULAR_MOVE);
    sort(begin(moveList), end(moveList));
    sort(begin(expectedMoves), end(expectedMoves));
    ASSERT(!memcmp(moveList, expectedMoves, sizeof(moveList[0]) * 256), "Incorrect bishop moves");

    return 1;
}

// Test bishopGen: bishop capture moves
int testBishopGenCaps() {
    cout << "Testing bishopGen with capture moves..." << endl;

    mv capList[256] = { 0 };
    mv * caps = capList;

    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   | P |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   | p |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   | b |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   | p |   |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+

    CBoard board("8/8/2P5/5p2/4b3/3p4/8/8", "b", "", "-", "0", "0");
    magics::populateBishopTable(E4);
    board.bishopGen(&caps, 0, false);
    mv expectedCaps[256] = { 0 };
    expectedCaps[0] = moves::make(E4, C6, BISHOP, PAWN, NONE, NONE, REGULAR_MOVE);
    sort(begin(capList), end(capList));
    sort(begin(expectedCaps), end(expectedCaps));
    ASSERT(!memcmp(capList, expectedCaps, sizeof(capList[0]) * 256), "Incorrect bishop captures");

    return 1;
}

// Test bishopGen: quen diagonal moves
int testBishopGenQueen() {
    cout << "Testing bishopGen with queen moves..." << endl;

    mv moveList[256] = { 0 };
    mv * moves = moveList;

    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   | p | p | p |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   | P |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   | P |   | Q |
    // +---+---+---+---+---+---+---+---+

    CBoard board("8/8/8/8/8/5ppp/5P2/5P1Q", "w", "", "-", "0", "0");
    magics::populateBishopTable(H1);
    board.bishopGen(&moves, 0, true);
    mv expectedMoves[256] = { 0 };
    expectedMoves[0] = moves::make(H1, G2, QUEEN, NONE, NONE, NONE, REGULAR_MOVE);
    expectedMoves[1] = moves::make(H1, F3, QUEEN, PAWN, NONE, NONE, REGULAR_MOVE);
    sort(begin(moveList), end(moveList));
    sort(begin(expectedMoves), end(expectedMoves));
    ASSERT(!memcmp(moveList, expectedMoves, sizeof(moveList[0]) * 256), "Incorrect queen moves");

    return 1;
}

// Test bishopGenPinned: bishops that are pinned
int testBishopGenPinned() {
    cout << "Testing bishopGenPinned..." << endl;

    mv moveList[256] = { 0 };
    mv * moves = moveList;

    // +---+---+---+---+---+---+---+---+
    // | b |   |   |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   | B |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |[K]|   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+

    CBoard board("b7/8/2B5/8/4K3/8/8/8", "w", "", "-", "0", "0");
    magics::populateBishopTable(C6);
    masks::generateOpposite();
    masks::generateInterceding();
    board.bishopGenPinned(&moves, exp_2(C6), E4, true);
    mv expectedMoves[256] = { 0 };
    expectedMoves[0] = moves::make(C6, A8, BISHOP, BISHOP, NONE, NONE, REGULAR_MOVE);
    expectedMoves[1] = moves::make(C6, B7, BISHOP, NONE, NONE, NONE, REGULAR_MOVE);
    expectedMoves[2] = moves::make(C6, D5, BISHOP, NONE, NONE, NONE, REGULAR_MOVE);
    sort(begin(moveList), end(moveList));
    sort(begin(expectedMoves), end(expectedMoves));
    ASSERT(!memcmp(moveList, expectedMoves, sizeof(moveList[0]) * 256), "Incorrect bishop pinned moves");

    return 1;
}

// Test rookGen: all rook moves
int testRookGen() {
    cout << "Testing rookGen with quiet moves..." << endl;

    mv moveList[256] = { 0 };
    mv * moves = moveList;

    // +---+---+---+---+---+---+---+---+
    // |   | p |   |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // | P | R |   |   | p |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   | P |   |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+

    CBoard board("1p6/PR2p3/8/1P6/8/8/8/8", "w", "", "-", "0", "0");
    magics::populateRookTable(B7);
    board.rookGen(&moves, 0, true);
    mv expectedMoves[256] = { 0 };
    expectedMoves[0] = moves::make(B7, B8, ROOK, PAWN, NONE, NONE, REGULAR_MOVE);
    expectedMoves[1] = moves::make(B7, C7, ROOK, NONE, NONE, NONE, REGULAR_MOVE);
    expectedMoves[2] = moves::make(B7, D7, ROOK, NONE, NONE, NONE, REGULAR_MOVE);
    expectedMoves[3] = moves::make(B7, E7, ROOK, PAWN, NONE, NONE, REGULAR_MOVE);
    expectedMoves[4] = moves::make(B7, B6, ROOK, NONE, NONE, NONE, REGULAR_MOVE);
    sort(begin(moveList), end(moveList));
    sort(begin(expectedMoves), end(expectedMoves));
    ASSERT(!memcmp(moveList, expectedMoves, sizeof(moveList[0]) * 256), "Incorrect rook moves");

    return 1;
}

// Test rookGen: rook capture moves
int testRookGenCaps() {
    cout << "Testing rookGen with capture moves..." << endl;

    mv capList[256] = { 0 };
    mv * caps = capList;

    // +---+---+---+---+---+---+---+---+
    // |   | p |   |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // | P | R |   |   | p |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   | P |   |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+

    CBoard board("1p6/PR2p3/8/1P6/8/8/8/8", "w", "", "-", "0", "0");
    magics::populateRookTable(B7);
    board.rookGen(&caps, 0, false);
    mv expectedCaps[256] = { 0 };
    expectedCaps[0] = moves::make(B7, B8, ROOK, PAWN, NONE, NONE, REGULAR_MOVE);
    expectedCaps[1] = moves::make(B7, E7, ROOK, PAWN, NONE, NONE, REGULAR_MOVE);
    sort(begin(capList), end(capList));
    sort(begin(expectedCaps), end(expectedCaps));
    ASSERT(!memcmp(capList, expectedCaps, sizeof(capList[0]) * 256), "Incorrect rook moves");

    return 1;
}

// Test rookGen: queen horizontal moves
int testRookGenQueen() {
    cout << "Testing rookGen with queen moves..." << endl;

    mv moveList[256] = { 0 };
    mv * moves = moveList;

    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   |   |   | P |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   |   |   | Q |
    // +---+---+---+---+---+---+---+---+

    CBoard board("8/8/8/8/8/8/7P/7Q", "w", "", "-", "0", "0");
    magics::populateRookTable(H1);
    board.rookGen(&moves, 0, true);
    mv expectedMoves[256] = { 0 };
    expectedMoves[0] = moves::make(H1, G1, QUEEN, NONE, NONE, NONE, REGULAR_MOVE);
    expectedMoves[1] = moves::make(H1, F1, QUEEN, NONE, NONE, NONE, REGULAR_MOVE);
    expectedMoves[2] = moves::make(H1, E1, QUEEN, NONE, NONE, NONE, REGULAR_MOVE);
    expectedMoves[3] = moves::make(H1, D1, QUEEN, NONE, NONE, NONE, REGULAR_MOVE);
    expectedMoves[4] = moves::make(H1, C1, QUEEN, NONE, NONE, NONE, REGULAR_MOVE);
    expectedMoves[5] = moves::make(H1, B1, QUEEN, NONE, NONE, NONE, REGULAR_MOVE);
    expectedMoves[6] = moves::make(H1, A1, QUEEN, NONE, NONE, NONE, REGULAR_MOVE);
    sort(begin(moveList), end(moveList));
    sort(begin(expectedMoves), end(expectedMoves));
    ASSERT(!memcmp(moveList, expectedMoves, sizeof(moveList[0]) * 256), "Incorrect horizontal queen moves");

    return 1;
}

// Test rookGenPinned: bishops that are pinned
int testRookGenPinned() {
    cout << "Testing rookGenPinned..." << endl;

    mv moveList[256] = { 0 };
    mv * moves = moveList;

    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // | r |   | R |   |[K]|   |   |   |
    // +---+---+---+---+---+---+---+---+

    CBoard board("8/8/8/8/8/8/8/r1R1K3", "w", "", "-", "0", "0");
    magics::populateRookTable(C1);
    masks::generateOpposite();
    masks::generateInterceding();
    board.rookGenPinned(&moves, exp_2(C1), E1, true);
    mv expectedMoves[256] = { 0 };
    expectedMoves[0] = moves::make(C1, A1, ROOK, ROOK, NONE, NONE, REGULAR_MOVE);
    expectedMoves[1] = moves::make(C1, B1, ROOK, NONE, NONE, NONE, REGULAR_MOVE);
    expectedMoves[2] = moves::make(C1, D1, ROOK, NONE, NONE, NONE, REGULAR_MOVE);
    sort(begin(moveList), end(moveList));
    sort(begin(expectedMoves), end(expectedMoves));
    ASSERT(!memcmp(moveList, expectedMoves, sizeof(moveList[0]) * 256), "Incorrect pinned rook moves");

    return 1;
}

// Test bishopPins for bishops and queens pinning pieces diagonally
int testBishopPins() {
    cout << "Testing bishopPins..." << endl;

    // +---+---+---+---+---+---+---+---+
    // | q |   |   |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   | b |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   | P |   | P |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |[K]|   |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   | P |   | P |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+

    CBoard board("q7/5b2/2P1P3/3K4/2P1P3/8/8/8", "w", "", "-", "0", "0");
    magics::populateBishopTables();
    BB pins = board.bishopPins(D5);
    BB expectedPins = exp_2(C6) | exp_2(E6);
    ASSERT(pins == expectedPins, "Wrong bishop pins");

    return 1;
}

// Test rookPins for rooks and queens pinning pieces horizontally/vertically
int testRookPins() {
    cout << "Testing rookPins..." << endl;

    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   | q |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   | P |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // | r |   | P |   |[K]| P |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+

    CBoard board("4q3/8/8/4P3/r1P1KP2/8/8/8", "w", "", "-", "0", "0");
    magics::populateRookTables();
    BB pins = board.rookPins(E4);
    BB expectedPins = exp_2(C4) | exp_2(E5);
    ASSERT(pins == expectedPins, "Wrong rook pins");

    return 1;
}

// Tests attackSetGen on a board with one of each piece type.
int testAttackSetGen() {
    cout << "Testing attackSetGen..." << endl;

    // +---+---+---+---+---+---+---+---+
    // |   |   |   |[k]|   |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   |   | P |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   |   | p | b |
    // +---+---+---+---+---+---+---+---+
    // | r | n |   |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   | q |   |   |
    // +---+---+---+---+---+---+---+---+
    // | p |   |   |   |   | P |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+

    CBoard board("3k4/6P1/6pb/rn6/8/5q2/p4P2/8", "w", "", "-", "0", "0");
    magics::populateBishopTables();
    magics::populateRookTables();
    BB attacks = board.attackSetGen(BLACK);
    BB expectedAttacks = 0b1010110011111110101101000101011110011110111110111001111001110001;
    ASSERT(attacks == expectedAttacks, "Wrong attack set");

    return 1;
}

// Tests kingGen with quiet moves, captures and castling.
int testKingMoves() {
    cout << "Testing kingGen with quiet moves and castling..." << endl;

    mv moveList[256] = { 0 };
    mv * moves = moveList;

    // +---+---+---+---+---+---+---+---+
    // |[k]|   |   |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // | R |   |   |   |[K]|   |   | R |
    // +---+---+---+---+---+---+---+---+

    CBoard board("k7/8/8/8/8/8/8/R3K2R", "w", "KQkq", "-", "0", "0");
    BB enemyAttacks = board.attackSetGen(BLACK);
    board.kingGen(&moves, E1, enemyAttacks, true);
    mv expectedMoves[256] = { 0 };
    expectedMoves[0] = moves::make(E1, D1, KING, NONE, NONE, NONE, REGULAR_MOVE);
    expectedMoves[1] = moves::make(E1, D2, KING, NONE, NONE, NONE, REGULAR_MOVE);
    expectedMoves[2] = moves::make(E1, E2, KING, NONE, NONE, NONE, REGULAR_MOVE);
    expectedMoves[3] = moves::make(E1, F2, KING, NONE, NONE, NONE, REGULAR_MOVE);
    expectedMoves[4] = moves::make(E1, F1, KING, NONE, NONE, NONE, REGULAR_MOVE);
    expectedMoves[5] = moves::make(E1, G1, KING, NONE, NONE, NONE, REGULAR_MOVE);
    expectedMoves[6] = moves::make(E1, C1, KING, NONE, NONE, NONE, REGULAR_MOVE);
    sort(begin(moveList), end(moveList));
    sort(begin(expectedMoves), end(expectedMoves));
    ASSERT(!memcmp(moveList, expectedMoves, sizeof(moveList[0]) * 256), "Incorrect king quiet moves");

    return 1;
}

// Tests kingGen with captures only.
int testKingMovesCaps() {
    cout << "Testing kingGen capatures only..." << endl;

    mv capList[256] = { 0 };
    mv * caps = capList;

    // +---+---+---+---+---+---+---+---+
    // |[k]|   |   |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   | p |   |   |
    // +---+---+---+---+---+---+---+---+
    // | R |   |   |   |[K]|   |   | R |
    // +---+---+---+---+---+---+---+---+

    CBoard board("k7/8/8/8/8/8/5p2/R3K2R", "w", "KQkq", "-", "0", "0");
    BB enemyAttacks = board.attackSetGen(BLACK);
    board.kingGen(&caps, E1, enemyAttacks, false);
    mv expectedMoves[256] = { 0 };
    expectedMoves[3] = moves::make(E1, F2, KING, PAWN, NONE, NONE, NONE);
    sort(begin(capList), end(capList));
    sort(begin(expectedMoves), end(expectedMoves));
    ASSERT(!memcmp(capList, expectedMoves, sizeof(capList[0]) * 256), "Incorrect king capture moves");

    return 1;
}

// Tests kingGen with quiet moves but no castling rights.
int testKingMovesNoCastling() {
    cout << "Testing kingGen with quiet moves and no castling..." << endl;

    mv moveList[256] = { 0 };
    mv * moves = moveList;

    // +---+---+---+---+---+---+---+---+
    // |[k]|   |   |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // | R |   |   |   |[K]|   |   | R |
    // +---+---+---+---+---+---+---+---+

    CBoard board("k7/8/8/8/8/8/8/R3K2R", "w", "kq", "-", "0", "0");
    BB enemyAttacks = board.attackSetGen(BLACK);
    board.kingGen(&moves, E1, enemyAttacks, true);
    mv expectedMoves[256] = { 0 };
    expectedMoves[0] = moves::make(E1, D1, KING, NONE, NONE, NONE, REGULAR_MOVE);
    expectedMoves[1] = moves::make(E1, D2, KING, NONE, NONE, NONE, REGULAR_MOVE);
    expectedMoves[2] = moves::make(E1, E2, KING, NONE, NONE, NONE, REGULAR_MOVE);
    expectedMoves[3] = moves::make(E1, F2, KING, NONE, NONE, NONE, REGULAR_MOVE);
    expectedMoves[4] = moves::make(E1, F1, KING, NONE, NONE, NONE, REGULAR_MOVE);
    sort(begin(moveList), end(moveList));
    sort(begin(expectedMoves), end(expectedMoves));
    ASSERT(!memcmp(moveList, expectedMoves, sizeof(moveList[0]) * 256), "Incorrect king quiet non-castling moves");

    return 1;
}

// Tests move generation to a given square
int testGenerateMovesTo() {
    cout << "Testing generateMovesTo on empty square..." << endl;

    mv moveList[256] = { 0 };
    mv * moves = moveList;

    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   | R |   | Q |   |
    // +---+---+---+---+---+---+---+---+
    // | k |   |   |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   | : |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   | P | p |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   | N | B |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |[K]|   |   |   |
    // +---+---+---+---+---+---+---+---+

    CBoard board("4R1Q1/k7/8/3Pp3/5NB1/8/8/4K3", "w", "", "e6", "0", "0");
    magics::populateBishopTables();
    magics::populateRookTables();
    board.generateMovesTo(&moves, E6, NONE, 0, 0);
    mv expectedMoves[256] = { 0 };
    expectedMoves[0] = moves::make(D5, E6, PAWN, PAWN, NONE, NONE, EN_PASSANT_CAP_W);
    expectedMoves[1] = moves::make(F4, E6, KNIGHT, NONE, NONE, NONE, REGULAR_MOVE);
    expectedMoves[2] = moves::make(E8, E6, ROOK, NONE, NONE, NONE, REGULAR_MOVE);
    expectedMoves[3] = moves::make(G8, E6, QUEEN, NONE, NONE, NONE, REGULAR_MOVE);
    expectedMoves[4] = moves::make(G4, E6, BISHOP, NONE, NONE, NONE, REGULAR_MOVE);
    sort(begin(moveList), end(moveList));
    sort(begin(expectedMoves), end(expectedMoves));
    ASSERT(!memcmp(moveList, expectedMoves, sizeof(moveList[0]) * 256), "Incorrect quiet movesTo moves");

    return 1;
}

// Tests capture generation to a given square
int testGenerateMovesToCaps() {
    cout << "Testing generateMovesTo on an occupied square..." << endl;

    mv moveList[256] = { 0 };
    mv * moves = moveList;

    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   | N |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   |   | B |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   | r | R |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   | P |[K]|   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+

    CBoard board("4N3/6B1/5rR1/4PK2/8/8/8/8", "w", "", "-", "0", "0");
    magics::populateBishopTables();
    magics::populateRookTables();
    board.generateMovesTo(&moves, F6, ROOK, 0, 0);
    mv expectedMoves[256] = { 0 };
    expectedMoves[0] = moves::make(E5, F6, PAWN, ROOK, NONE, NONE, REGULAR_MOVE);
    expectedMoves[1] = moves::make(E8, F6, KNIGHT, ROOK, NONE, NONE, REGULAR_MOVE);
    expectedMoves[2] = moves::make(G6, F6, ROOK, ROOK, NONE, NONE, REGULAR_MOVE);
    expectedMoves[3] = moves::make(G7, F6, BISHOP, ROOK, NONE, NONE, REGULAR_MOVE);
    sort(begin(moveList), end(moveList));
    sort(begin(expectedMoves), end(expectedMoves));
    ASSERT(!memcmp(moveList, expectedMoves, sizeof(moveList[0]) * 256), "Incorrect capture movesTo moves");

    return 1;
}

int main() {
    int t = 0;

    t += testPawnGen();
    t += testPawnGenPromotions();
    t += testPawnCaps();
    t += testPawnCapsEnPassant();
    t += testPawnGenPinned();
    t += testKnightGen();
    t += testKnightCaps();
    t += testBishopGen();
    t += testBishopGenCaps();
    t += testBishopGenQueen();
    t += testBishopGenPinned();
    t += testRookGen();
    t += testRookGenCaps();
    t += testRookGenQueen();
    t += testRookGenPinned();
    t += testBishopPins();
    t += testRookPins();
    t += testAttackSetGen();
    t += testKingMoves();
    t += testKingMovesCaps();
    t += testKingMovesNoCastling();
    t += testGenerateMovesTo();
    t += testGenerateMovesToCaps();

    cout << endl;
    cout << t << " test(s) OK" << endl;
    return 0;
}
