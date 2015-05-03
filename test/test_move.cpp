
#include <string.h>
#include <map>
#include "assert.h"
#include "../src/move.cpp"
#include "../src/cboard_defs.h"
#include "../src/bb.cpp"

using namespace std;

int testEndToEnd() {
    cout << "Testing move namespace end to end..." << endl;

    mv m = move::make(E2, E4, PAWN, 0, 3, 5, 12);
    ASSERT(move::source(m) == E2, "Corrupted move");
    ASSERT(move::dest(m) == E4, "Corrupted move");
    ASSERT(move::attacker(m) == PAWN, "Corrupted move");
    ASSERT(move::defender(m) == 0, "Corrupted move");
    ASSERT(move::enPassant(m) == 3, "Corrupted move");
    ASSERT(move::castling(m) == 5, "Corrupted move");
    ASSERT(move::special(m) == 12, "Corrupted move");

    return 1;
}

int main() {
    int t = 0;

    t += testEndToEnd();

    cout << endl;
    cout << t << " test(s) OK" << endl;
    return 0;
}
