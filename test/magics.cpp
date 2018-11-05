#define _TEST 1

#include "../src/magics.h"

#include <algorithm>
#include <iostream>
#include <vector>

#include "../src/board.h"
#include "../src/inlines.h"
#include "../src/masks.h"
#include "../src/squares.h"
#include "assert.h"

using namespace exacto;
using namespace std;

// Tests GenerateSubsets with a basic example.
int testGenerateSubsets() {
  cout << "Testing GenerateSubsets" << endl;

  Bitboard b = exp_2(5) | exp_2(45) | exp_2(63);
  vector<Bitboard> subsets;
  vector<Bitboard> expectedSubsets = {
      0,
      exp_2(5),
      exp_2(45),
      exp_2(63),
      exp_2(5) | exp_2(45),
      exp_2(5) | exp_2(63),
      exp_2(45) | exp_2(63),
      b,
  };
  magics::GenerateSubsets(b, &subsets);
  sort(begin(subsets), end(subsets));
  sort(begin(expectedSubsets), end(expectedSubsets));
  ASSERT(subsets == expectedSubsets, "Wrong subsets");

  return 1;
}

// Very simple test case for bishop magics
int testBishopMagics() {
  cout << "Testing bishop magics" << endl;

  Board board("1b6/8/8/4P3/8/8/8/8", "b", "", "-");
  Bitboard expectedMoves = exp_2(A7) | exp_2(C7) | exp_2(D6) | exp_2(E5);
  magics::PopulateBishopTable(B8);
  Bitboard moves = magics::BishopMoves(B8, board.occupied);
  ASSERT(moves == expectedMoves, "Bishop magic test failed");

  return 1;
}

// Very simple test case for rook magics
int testRookMagics() {
  cout << "Testing bishop magics" << endl;

  Board board("R7/P7/8/8/8/8/8/8", "b", "", "-");
  Bitboard expectedMoves = exp_2(A7);
  for (ind square = B8; square >= H8; square--) {
    expectedMoves |= exp_2(square);
  }
  magics::PopulateRookTable(A8);
  Bitboard hash_key = magics::HashRook(board.occupied & ~exp_2(A8), A8);
  hash_key = magics::HashRook(36028797018963968, A8);
  Bitboard moves = magics::ROOK_MOVES[A8][hash_key];
  ASSERT(moves == expectedMoves, "Rook magic test failed");

  return 1;
}

int main() {
  int t = 0;

  t += testGenerateSubsets();
  t += testBishopMagics();
  t += testRookMagics();

  cout << endl;
  cout << t << " test(s) OK" << endl;
  return 0;
}
