#include "../src/moves.h"

#include "../src/squares.h"

#include "assert.h"

using namespace exacto;
using namespace std;

// Tests of the move namespace: creating and unpacking moves.
int TestEndToEnd() {
  cout << "Testing move namespace end to end..." << endl;

  Move m = moves::make(E4, D5, PAWN, PAWN, 3, 5, 12);
  ASSERT(moves::source(m) == E4, "Corrupted move");
  ASSERT(moves::dest(m) == D5, "Corrupted move");
  ASSERT(moves::attacker(m) == PAWN, "Corrupted move");
  ASSERT(moves::defender(m) == PAWN, "Corrupted move");
  ASSERT(moves::en_passant(m) == 3, "Corrupted move");
  ASSERT(moves::castling(m) == 5, "Corrupted move");
  ASSERT(moves::special(m) == 12, "Corrupted move");

  return 1;
}

// Tests that EncodeCastling and DecodeCastling are inverses.
int TestCastlingEncoding() {
  cout << "Testing EncodeCastling/DecodeCastling" << endl;

  for (ind i = 0; i < 15; i++) {
    Bitboard decoded = moves::DecodeCastling(i);
    Move encoded = moves::EncodeCastling(decoded);
    ASSERT(moves::castling(encoded) == i,
           "Bad castling encoding for bitstring " + to_string((int)i));
  }

  return 1;
}

int main() {
  int t = 0;

  t += TestEndToEnd();
  t += TestCastlingEncoding();

  cout << endl;
  cout << t << " test(s) OK" << endl;
  return 0;
}
