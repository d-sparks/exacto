#define _TEST 1

#include <string.h>
#include <map>
#include "assert.h"
#include "../src/cboard.cpp"

using namespace std;

// testMovePiece simply checks that the given pieces get moved on the board. Assumes correctness of
// CBoard::operator== and setBoard.
int testMovePiece() {
    cout << "Testing CBoard::movePiece..." << endl;
    CBoard board1, board2;

    board1.movePiece(WHITE, PAWN, E2, E4, exp_2(E2), exp_2(E4));
    board2.setBoard("rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR");
    ASSERT(board1 == board2, "Move gave wrong result");

    board1.movePiece(BLACK, PAWN, E7, E5, exp_2(E7), exp_2(E5));
    board2.setBoard("rnbqkbnr/pppp1ppp/8/4p3/4P3/8/PPPP1PPP/RNBQKBNR");
    ASSERT(board1 == board2, "Move gave wrong result");

    return 1;
}

// testKillPiece checks that piece removal works. Assumes correctness of CBoard::operator== and
// setBoard.
int testKillPiece() {
    cout << "Testing CBoard::killPiece..." << endl;
    CBoard board1, board2;

    board1.setBoard("8/8/8/8/4P3/8/8/8");
    board2.setBoard("8/8/8/8/8/8/8/8");
    board1.killPiece(WHITE, PAWN, E4, exp_2(E4));
    ASSERT(board1 == board2, "killPiece failed");

    board1.setBoard("8/8/8/8/4b3/8/8/8");
    board2.setBoard("8/8/8/8/8/8/8/8");
    board1.killPiece(BLACK, BISHOP, E4, exp_2(E4));
    ASSERT(board1 == board2, "killPiece failed");

    return 1;
}

// testMakePiece checks that creating additional pieces works. Assumes correctness of
// CBoard::operator== and setBoard.
int testMakePiece() {
    cout << "Testing CBoard::makePiece..." << endl;
    CBoard board1, board2;

    board1.setBoard("8/8/8/8/8/8/8/8");
    board2.setBoard("8/8/8/8/4N3/8/8/8");
    board1.makePiece(WHITE, KNIGHT, E4, exp_2(E4));
    ASSERT(board1 == board2, "makePiece failed");

    board1.setBoard("8/8/8/8/8/8/8/8");
    board2.setBoard("8/8/8/8/4r3/8/8/8");
    board1.makePiece(BLACK, ROOK, E4, exp_2(E4));
    ASSERT(board1 == board2, "makePiece failed");

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
