#include "assert.h"
#include "../src/cgame.cpp"
#include "../src/SEE.cpp"

using namespace std;

// Greatcomment
int testMakeMoveUnmakeMove() {
    cout << "Testing that makeMove and unmakeMove are inverses..." << endl;

    // +---+---+---+---+---+---+---+---+
    // | r |   |   | q |[k]|   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   | P |   |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   | : |   |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   | P | p |   |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   | P |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   | N |   |   |   | b |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   | P | P | P |
    // +---+---+---+---+---+---+---+---+
    // | R |   |   |   |[K]|   |   | R |
    // +---+---+---+---+---+---+---+---+

    CGame game1("r2qk3/1P6/8/2Pp4/4P3/1N3b2/5PPP/R3K2R", "w", "KQq", "-");
    CGame game2 = game1;
    mv moveList[256] = { 0 };
    game1.moveGen(moveList, true);
    for(mv* move = moveList; *move; move++) {
        BB sourceBB = exp_2(moves::source(*move));
        ind attacker = moves::attacker(*move);
        SEE::makeMove(&game1, sourceBB, attacker);
        SEE::unmakeMove(&game1, sourceBB, attacker);
        ASSERT(game1 == game2, "unmakeMove didn't undo makeMove: " + moves::algebraic(*move));
    }

    return 1;
}

// Tests that SEE gives appropriate values
int testSEEValues() {
    cout << "Testing that SEE gives the right values..." << endl;

    // +---+---+---+---+---+---+---+---+
    // |   |   |   | r |[k]| r |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   | n |   |   | P |   | B |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // | P |   | P |[K]|   |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+
    // |   |   |   |   |   |   |   |   |
    // +---+---+---+---+---+---+---+---+

    pair<mv,int16_t> moves[5] = {
        {moves::make(C4, C5, PAWN, NONE, NONE, NONE, NONE), 0},
        {moves::make(A4, A5, PAWN, NONE, NONE, NONE, NONE), -PAWN_VAL},
        {moves::make(D4, C5, KING, NONE, NONE, NONE, NONE), -MATESCORE},
        {moves::make(E7, D8, PAWN, ROOK, NONE, NONE, PROMOTE_QUEEN), ROOK_VAL - PAWN_VAL},
        {moves::make(E7, F8, PAWN, ROOK, NONE, NONE, PROMOTE_QUEEN), ROOK_VAL}
    };

    CGame game("3rkr2/1n2P1B1/8/8/P1PK4/8/8/8", "w", "-", "-");
    CGame gameRef("3rkr2/1n2P1B1/8/8/P1PK4/8/8/8", "w", "-", "-");
    for(int i = 0; i < 5; i++) {
        mv move = moves[i].first;
        int16_t expectedScore = moves[i].second;
        int16_t score = SEE::see(&game, move);
        ASSERT(game == gameRef, "SEE corrupted board state for move " + moves::algebraic(move));
        ASSERT(score == expectedScore, "SEE miscalculated " + moves::algebraic(move));
    }

    return 1;
}

int main() {
    int t = 0;

    magics::populateBishopTables();
    magics::populateRookTables();
    masks::init();

    t += testMakeMoveUnmakeMove();
    t += testSEEValues();

    cout << endl;
    cout << t << " test(s) OK" << endl;
    return 0;
}

