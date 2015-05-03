#define _TEST 1

#include <string.h>
#include <map>
#include "assert.h"
#include "../src/cboard.cpp"

using namespace std;

int testMovePiece() {
    cout << "Testing CBoard::movePiece..." << endl;
    CBoard board1, board2;

    board1.movePiece(WHITE, PAWN, E2, E4, exp_2(E2), exp_2(E4));
    board2.setBoard("rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR", "w", "KQkq", "-", "0", "0");
    ASSERT(board1 == board2, "Move gave wrong result");

    return 1;
}

int testKillPiece() {
    cout << "Testing CBoard::killPiece..." << endl;
    CBoard board1, board2;

    board1.setBoard("8/8/8/8/4P3/8/8/8", "w", "KQkq", "-", "0", "0");
    board2.setBoard("8/8/8/8/8/8/8/8", "w", "KQkq", "-", "0", "0");
    board1.killPiece(WHITE, PAWN, E4, exp_2(E4));
    ASSERT(board1 == board2, "killPiece failed");

    return 1;
}

int testMakePiece() {
    cout << "Testing CBoard::makePiece..." << endl;
    CBoard board1, board2;

    board1.setBoard("8/8/8/8/8/8/8/8", "w", "KQkq", "-", "0", "0");
    board2.setBoard("8/8/8/8/4P3/8/8/8", "w", "KQkq", "-", "0", "0");
    board1.makePiece(WHITE, PAWN, E4, exp_2(E4));
    ASSERT(board1 == board2, "killPiece failed");

    return 1;
}

int main() {
    int t = 0;

    t += testMovePiece();
    t += testKillPiece();
    t += testMakePiece();

    cout << endl;
    cout << t << " test(s) OK" << endl;
    return 0;
}
