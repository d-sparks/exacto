#define _TEST 1

#include <string.h>
// #include <map>
#include "assert.h"
#include "../src/cgame.cpp"
#include "../src/moves.cpp"
#include "../src/magics.cpp"

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

    CGame game1("r2qk3/1P6/8/2Pp4/4P3/1N3b2/5PPP/R3K2R", "w", "KQq", "D6");
    CGame game2 = game1;
    magics::populateBishopTables();
    magics::populateRookTables();
    mv moveList[256] = { 0 };
    game1.moveGen(moveList, true);
    for(mv * move = moveList; *move; move++) {
        game1.makeMove(move);
        game1.unmakeMove(*move);
        ASSERT(game1 == game2, "unmakeMove didn't undo makeMove: " + moves::algebraic(*move));
    }

    return 1;
}

int main() {
    int t = 0;

    t += testMakeMoveUnmakeMove();

    cout << endl;
    cout << t << " test(s) OK" << endl;
    return 0;
}
