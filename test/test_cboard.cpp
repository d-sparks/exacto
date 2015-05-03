#define _TEST 1

#include <string.h>
#include <map>
#include "assert.h"
#include "../src/cboard.cpp"

using namespace std;

int testMovePiece() {
    cout << "Testing CBoard::movePiece..." << endl;
    CBoard board1, board2;

    // board1.movePiece(E2, E4);
    // board2.setBoard("rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR", "b", "KQkq", "-", "0", "0");
    // ASSERT(board1 == board2, "Move gave wrong result");

    return 1;
}

int main() {
    int t = 0;

    t += testMovePiece();

    cout << endl;
    cout << t << " test(s) OK" << endl;
    return 0;
}
