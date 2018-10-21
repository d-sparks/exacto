#include "../src/masks.h"
#include "../src/bitboard.h"
#include "../src/squares.h"
#include "assert.h"

using namespace exacto;

// Test GenerateInterceding
int testGenerateInterceding() {
  cout << "Testing GenerateInterceding..." << endl;

  Bitboard expectedA8H1 =
      0b0000000001000000001000000001000000001000000001000000001000000000;
  Bitboard expectedE4G4 =
      0b0000000000000000000000000000000000000100000000000000000000000000;
  Bitboard expectedE5G4 = 0;
  masks::GenerateInterceding();
  ASSERT(masks::INTERCEDING[A8][H1] == expectedA8H1,
         "Wrong 'interceding squares' mask for A8,H1");
  ASSERT(masks::INTERCEDING[H1][A8] == expectedA8H1,
         "Wrong 'interceding squares' mask for H1,A8");
  ASSERT(masks::INTERCEDING[E4][G4] == expectedE4G4,
         "Wrong 'interceding squares' mask for E4,G4");
  ASSERT(masks::INTERCEDING[G4][E4] == expectedE4G4,
         "Wrong 'interceding squares' mask for G4,E4");
  ASSERT(masks::INTERCEDING[E5][G4] == expectedE5G4,
         "Wrong 'interceding squares' mask for E5,G4");
  ASSERT(masks::INTERCEDING[G4][E5] == expectedE5G4,
         "Wrong 'interceding squares' mask for G4,E5");

  return 1;
}

// Test GenerateOpposite
int testGenerateOpposite() {
  cout << "Testing GenerateOpposite..." << endl;

  Bitboard expectedA8B7 =
      0b0000000000000000001000000001000000001000000001000000001000000001;
  Bitboard expectedF1G1 =
      0b0000000000000000000000000000000000000000000000000000000000000001;
  Bitboard expectedE5G4 = 0;
  masks::GenerateOpposite();
  ASSERT(masks::OPPOSITE[A8][B7] == expectedA8B7,
         "Wrong 'opposite squares' mask for A8,B7");
  ASSERT(masks::OPPOSITE[F1][G1] == expectedF1G1,
         "Wrong 'opposite squares' mask for F1,G1");
  ASSERT(masks::OPPOSITE[E5][G4] == expectedE5G4,
         "Wrong 'opposite squares' mask for E5,G4");
  return 1;
}

int main() {
  int t = 0;

  t += testGenerateInterceding();
  t += testGenerateOpposite();

  cout << endl;
  cout << t << " test(s) OK" << endl;
  return 0;
}
