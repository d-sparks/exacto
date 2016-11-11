#include "assert.h"
#include "../src/cexacto.cpp"
#include "../src/moves.cpp"

using namespace std;

// Tests that moves are sorted appropriately
int testSortMoves() {
    cout << "Testing that sort moves sorts correctly..." << endl;

    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |[k]| r |   | q |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   |   | B |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   |   |   | B |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   |   | P |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   |   | P |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   | p |   |   | p | p |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   |   | n |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   | b | R |[K]|
    // +---+---+---+---+---+---+---+---+

    CGame game("4kr1q/6B1/7B/6P1/6P1/2p2pp1/6n1/5bRK", "w", "-", "-");
    CGame gameRef("4kr1q/6B1/7B/6P1/6P1/2p2pp1/6n1/5bRK", "w", "-", "-");
    CExacto exacto(game);
    mv moveList[256] = { 0 };
    game.moveGen(moveList, true);
    pair<mv, int16_t> expected[9] = {
        {moves::make(G7, D4, BISHOP, NONE, NONE, NONE, NONE), MATESCORE},
        {moves::make(G7, H8, BISHOP, QUEEN, NONE, NONE, NONE), QUEEN_VAL - BISHOP_VAL},
        {moves::make(G7, F8, BISHOP, ROOK, NONE, NONE, NONE), ROOK_VAL},
        {moves::make(G1, F1, ROOK, BISHOP, NONE, NONE, NONE), BISHOP_VAL},
        {moves::make(G5, G6, PAWN, NONE, NONE, NONE, NONE), 0},
        {moves::make(G7, F6, BISHOP, NONE, NONE, NONE, NONE), 0},
        {moves::make(G7, C3, BISHOP, PAWN, NONE, NONE, NONE), PAWN_VAL - BISHOP_VAL},
        {moves::make(G1, G2, ROOK, KNIGHT, NONE, NONE, NONE), KNIGHT_VAL - ROOK_VAL},
        {moves::make(G7, E5, BISHOP, NONE, NONE, NONE, NONE), -BISHOP_VAL}
    };
    exacto.hash.record(game.hashKey, expected[0].first, 1, HASH_EXACT, 0);
    exacto.sortMoves(&game, moveList);
    for(int i = 0; i < 8; i++) {
        cout << "..." << moves::algebraic(moveList[i]) << " should be " << moves::algebraic(expected[i].first) << endl;
        ASSERT(moveList[i] == expected[i].first, "Bad move sort");
    }
    ASSERT(game == gameRef, "Corrupted position");

    return 1;
}

int main() {
    int t = 0;

    magics::populateBishopTables();
    magics::populateRookTables();
    masks::init();

    t += testSortMoves();

    cout << endl;
    cout << t << " test(s) OK" << endl;
    return 0;
}

