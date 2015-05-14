
#include <string.h>
#include <map>
#include "assert.h"
#include "../src/pieces.h"
#include "../src/moves.cpp"
#include "../src/bb.cpp"

using namespace std;

// Tests of the move namespace: creating and unpacking moves.
int testEndToEnd() {
    cout << "Testing move namespace end to end..." << endl;

    mv m = moves::make(E4, D5, PAWN, PAWN, 3, 5, 12);
    ASSERT(moves::source(m) == E4, "Corrupted move");
    ASSERT(moves::dest(m) == D5, "Corrupted move");
    ASSERT(moves::attacker(m) == PAWN, "Corrupted move");
    ASSERT(moves::defender(m) == PAWN, "Corrupted move");
    ASSERT(moves::enPassant(m) == 3, "Corrupted move");
    ASSERT(moves::castling(m) == 5, "Corrupted move");
    ASSERT(moves::special(m) == 12, "Corrupted move");

    return 1;
}

int main() {
    int t = 0;

    t += testEndToEnd();

    cout << endl;
    cout << t << " test(s) OK" << endl;
    return 0;
}
