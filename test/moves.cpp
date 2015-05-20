#include "assert.h"
#include "../src/pieces.h"
#include "../src/moves.cpp"

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

// Tests that castlingEncode and castlingDecode are inverses.
int testCastlingEncoding() {
    cout << "Testing castlingEncode/castlingDecode" << endl;

    for(ind i = 0; i < 15; i++) {
        BB decoded = moves::castlingDecode(i);
        mv encoded = moves::castlingEncode(decoded);
        ASSERT(moves::castling(encoded) == i, "Bad castling encoding for bitstring " + to_string((int)i));
    }

    return 1;
}

int main() {
    int t = 0;

    t += testEndToEnd();
    t += testCastlingEncoding();

    cout << endl;
    cout << t << " test(s) OK" << endl;
    return 0;
}
