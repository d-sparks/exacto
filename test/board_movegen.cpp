#define _TEST 1

#include "../src/board.h"

#include <algorithm>
#include <cstring>

#include "../src/bitboard.h"
#include "../src/defines.h"
#include "../src/inlines.h"
#include "../src/magics.h"
#include "../src/masks.h"
#include "../src/squares.h"
#include "assert.h"

using namespace exacto;
using namespace std;

// Test pawnGen: non-capture pawn move generation
int TestPawnGen() {
  cout << "Testing pawnGen..." << endl;

  Move move_list[256] = {0};
  Move* moves = move_list;

  // +---+---+---+---+---+---+---+---+
  // | r | n | b |   |[k]| b |   | r |
  // +---+---+---+---+---+---+---+---+
  // |   | p |   |   |   | p |   | p |
  // +---+---+---+---+---+---+---+---+
  // | p |   |   |   | p | p |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   | p |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   | N |   | P |   |   |
  // +---+---+---+---+---+---+---+---+
  // | q |   | N |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // | P |   | P | Q |   |   | P | P |
  // +---+---+---+---+---+---+---+---+
  // |   | R |   |   |[K]| B |   | R |
  // +---+---+---+---+---+---+---+---+

  Board board("rnb1kb1r/1p3p1p/p3pp2/4p3/3N1P2/q1N5/P1PQ2PP/1R2KB1R", "w",
              "Kkq");
  board.pawnGen(&moves, 0, true);
  Move expected_moves[256] = {0};
  expected_moves[0] = moves::make(H2, H3, PAWN, NONE, NONE, NONE, REGULAR_MOVE);
  expected_moves[1] =
      moves::make(H2, H4, PAWN, NONE, NONE, NONE, DOUBLE_PAWN_MOVE_W);
  expected_moves[2] = moves::make(G2, G3, PAWN, NONE, NONE, NONE, REGULAR_MOVE);
  expected_moves[3] =
      moves::make(G2, G4, PAWN, NONE, NONE, NONE, DOUBLE_PAWN_MOVE_W);
  expected_moves[4] = moves::make(F4, F5, PAWN, NONE, NONE, NONE, REGULAR_MOVE);
  expected_moves[5] = moves::make(F4, E5, PAWN, PAWN, NONE, NONE, REGULAR_MOVE);
  sort(begin(move_list), end(move_list));
  sort(begin(expected_moves), end(expected_moves));
  ASSERT(!memcmp(move_list, expected_moves, sizeof(move_list[0]) * 256),
         "Incorrect pawn move generation");

  return 1;
}

// Test pawnGen: pawn promotion
int TestPawnGenPromotions() {
  cout << "Testing pawnGen for pawn promotion..." << endl;

  Move move_list[256] = {0};
  Move* moves = move_list;

  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   |   | p |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   | R |   |
  // +---+---+---+---+---+---+---+---+

  Board board("8/8/8/8/8/8/7p/6R1", "b", "");
  board.pawnGen(&moves, 0, true);
  Move expected_moves[256] = {0};
  for (ind special = PROMOTE_KNIGHT; special <= PROMOTE_QUEEN; ++special) {
    expected_moves[special] =
        moves::make(H2, H1, PAWN, NONE, NONE, NONE, special);
    expected_moves[100 + special] =
        moves::make(H2, G1, PAWN, ROOK, NONE, NONE, special);
  }
  sort(begin(move_list), end(move_list));
  sort(begin(expected_moves), end(expected_moves));
  ASSERT(!memcmp(move_list, expected_moves, sizeof(move_list[0]) * 256),
         "Incorrect pawn promotion generation");

  return 1;
}

// Test pawnCaps: pawn capture moves
int TestPawnCaps() {
  cout << "Testing pawnCaps..." << endl;

  Move cap_list[256] = {0};
  Move* caps = cap_list;

  // +---+---+---+---+---+---+---+---+
  // | r | n | b |   |[k]| b |   | r |
  // +---+---+---+---+---+---+---+---+
  // |   | p |   |   |   | p |   | p |
  // +---+---+---+---+---+---+---+---+
  // | p |   |   |   | p | p |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   | p |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   | N |   | P |   |   |
  // +---+---+---+---+---+---+---+---+
  // | q |   | N |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // | P |   | P | Q |   |   | P | P |
  // +---+---+---+---+---+---+---+---+
  // |   | R |   |   |[K]| B |   | R |
  // +---+---+---+---+---+---+---+---+

  Board board("rnb1kb1r/1p3p1p/p3pp2/4p3/3N1P2/q1N5/P1PQ2PP/1R2KB1R", "w",
               "Kkq");
  board.pawnCaps(&caps, 0);
  Move expected_caps[256] = {0};
  expected_caps[0] = moves::make(F4, E5, PAWN, PAWN, NONE, NONE, REGULAR_MOVE);
  sort(begin(cap_list), end(cap_list));
  sort(begin(expected_caps), end(expected_caps));
  ASSERT(!memcmp(cap_list, expected_caps, sizeof(cap_list[0]) * 256),
         "Incorrect pawn captures generation");

  return 1;
}

// Test pawnCaps: pawn en_passant captures
int TestPawnCapsEnPassant() {
  cout << "Testing pawnCaps for en_passant..." << endl;

  Move cap_list[256] = {0};
  Move* caps = cap_list;

  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   | : |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   | P | p |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |[K]|   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+

  Board board("8/8/8/3Pp3/8/8/8/K7", "w", "", "E6");
  board.pawnCaps(&caps, 0);
  Move expected_caps[256] = {0};
  expected_caps[0] = moves::make(D5, E6, PAWN, PAWN, NONE, NONE, EN_PASSANT_CAP);
  sort(begin(cap_list), end(cap_list));
  sort(begin(expected_caps), end(expected_caps));
  ASSERT(!memcmp(cap_list, expected_caps, sizeof(cap_list[0]) * 256),
         "Incorrect en passant pawn captures generation");

  return 1;
}

// Test pawnGenPinned
int TestPawnGenPinned() {
  cout << "Testing pawnGenPinned..." << endl;

  Move move_list[256] = {0};
  Move* moves = move_list;

  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // | b |   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   | p | P |   |   |   | r |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   |   | r |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   | P |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   |[K]|   |
  // +---+---+---+---+---+---+---+---+

  Board board("8/b7/8/1pP3r1/8/7r/6P1/6K1", "w", "", "B6");
  masks::GenerateOpposite();
  board.pawnGenPinned(&moves, exp_2(C5) | exp_2(G2), G1, true);
  Move expected_moves[256] = {0};
  expected_moves[0] =
      moves::make(C5, B6, PAWN, PAWN, NONE, NONE, EN_PASSANT_CAP);
  expected_moves[1] = moves::make(G2, G3, PAWN, NONE, NONE, NONE, REGULAR_MOVE);
  expected_moves[2] =
      moves::make(G2, G4, PAWN, NONE, NONE, NONE, DOUBLE_PAWN_MOVE_W);
  sort(begin(move_list), end(move_list));
  sort(begin(expected_moves), end(expected_moves));
  ASSERT(!memcmp(move_list, expected_moves, sizeof(move_list[0]) * 256),
         "Incorrect pinned pawn move generation");

  return 1;
}

// Test knightGen: all knight moves
int TestKnightGen() {
  cout << "Testing knightGen..." << endl;

  Move move_list[256] = {0};
  Move* moves = move_list;

  // +---+---+---+---+---+---+---+---+
  // | r | n | b |   |[k]| b |   | r |
  // +---+---+---+---+---+---+---+---+
  // |   | p |   |   |   | p |   | p |
  // +---+---+---+---+---+---+---+---+
  // | p |   |   |   | p | p |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   | p |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   | N |   | P |   |   |
  // +---+---+---+---+---+---+---+---+
  // | q |   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // | P |   | P | Q |   |   | P | P |
  // +---+---+---+---+---+---+---+---+
  // | N | R |   |   |[K]| B |   | R |
  // +---+---+---+---+---+---+---+---+

  Board board("rnb1kb1r/1p3p1p/p3pp2/4p3/3N1P2/q7/P1PQ2PP/NR2KB1R", "w",
               "Kkq");
  board.knightGen(&moves, 0, true);
  Move expected_moves[256] = {0};
  expected_moves[0] =
      moves::make(A1, B3, KNIGHT, NONE, NONE, NONE, REGULAR_MOVE);
  expected_moves[1] =
      moves::make(D4, B3, KNIGHT, NONE, NONE, NONE, REGULAR_MOVE);
  expected_moves[2] =
      moves::make(D4, B5, KNIGHT, NONE, NONE, NONE, REGULAR_MOVE);
  expected_moves[3] =
      moves::make(D4, C6, KNIGHT, NONE, NONE, NONE, REGULAR_MOVE);
  expected_moves[4] =
      moves::make(D4, E6, KNIGHT, PAWN, NONE, NONE, REGULAR_MOVE);
  expected_moves[5] =
      moves::make(D4, F5, KNIGHT, NONE, NONE, NONE, REGULAR_MOVE);
  expected_moves[6] =
      moves::make(D4, F3, KNIGHT, NONE, NONE, NONE, REGULAR_MOVE);
  expected_moves[7] =
      moves::make(D4, E2, KNIGHT, NONE, NONE, NONE, REGULAR_MOVE);
  sort(begin(move_list), end(move_list));
  sort(begin(expected_moves), end(expected_moves));
  ASSERT(!memcmp(move_list, expected_moves, sizeof(move_list[0]) * 256),
         "Incorrect knight move generation");

  return 1;
}

// Test knightCaps: knight capture moves only
int TestKnightCaps() {
  cout << "Testing knightCaps..." << endl;

  Move cap_list[256] = {0};
  Move* caps = cap_list;

  // +---+---+---+---+---+---+---+---+
  // | r | n | b |   |[k]| b |   | r |
  // +---+---+---+---+---+---+---+---+
  // |   | p |   |   |   | p |   | p |
  // +---+---+---+---+---+---+---+---+
  // | p |   |   |   | p | p |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   | p |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   | N |   | P |   |   |
  // +---+---+---+---+---+---+---+---+
  // | q |   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // | P |   | P | Q |   |   | P | P |
  // +---+---+---+---+---+---+---+---+
  // | N | R |   |   |[K]| B |   | R |
  // +---+---+---+---+---+---+---+---+

  Board board("rnb1kb1r/1p3p1p/p3pp2/4p3/3N1P2/q7/P1PQ2PP/NR2KB1R", "w",
               "Kkq");
  board.knightGen(&caps, 0, false);
  Move expected_caps[256] = {0};
  expected_caps[0] = moves::make(D4, E6, KNIGHT, PAWN, NONE, NONE, REGULAR_MOVE);
  ASSERT(!memcmp(cap_list, expected_caps, sizeof(cap_list[0]) * 256),
         "Incorrect knight capture generation");

  return 1;
}

// Test bishopGen: all bishop moves
int TestBishopGen() {
  cout << "Testing bishopGen with quiet moves..." << endl;

  Move move_list[256] = {0};
  Move* moves = move_list;

  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   | P |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   | p |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   | b |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   | p |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+

  Board board("8/8/2P5/5p2/4b3/3p4/8/8", "b", "");
  magics::populateBishopTable(E4);
  board.bishopGen(&moves, 0, true);
  Move expected_moves[256] = {0};
  expected_moves[0] =
      moves::make(E4, D5, BISHOP, NONE, NONE, NONE, REGULAR_MOVE);
  expected_moves[1] =
      moves::make(E4, C6, BISHOP, PAWN, NONE, NONE, REGULAR_MOVE);
  expected_moves[2] =
      moves::make(E4, F3, BISHOP, NONE, NONE, NONE, REGULAR_MOVE);
  expected_moves[3] =
      moves::make(E4, G2, BISHOP, NONE, NONE, NONE, REGULAR_MOVE);
  expected_moves[4] =
      moves::make(E4, H1, BISHOP, NONE, NONE, NONE, REGULAR_MOVE);
  sort(begin(move_list), end(move_list));
  sort(begin(expected_moves), end(expected_moves));
  ASSERT(!memcmp(move_list, expected_moves, sizeof(move_list[0]) * 256),
         "Incorrect bishop moves");

  return 1;
}

// Test bishopGen: bishop capture moves
int TestBishopGenCaps() {
  cout << "Testing bishopGen with capture moves..." << endl;

  Move cap_list[256] = {0};
  Move* caps = cap_list;

  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   | P |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   | p |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   | b |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   | p |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+

  Board board("8/8/2P5/5p2/4b3/3p4/8/8", "b", "");
  magics::populateBishopTable(E4);
  board.bishopGen(&caps, 0, false);
  Move expected_caps[256] = {0};
  expected_caps[0] = moves::make(E4, C6, BISHOP, PAWN, NONE, NONE, REGULAR_MOVE);
  sort(begin(cap_list), end(cap_list));
  sort(begin(expected_caps), end(expected_caps));
  ASSERT(!memcmp(cap_list, expected_caps, sizeof(cap_list[0]) * 256),
         "Incorrect bishop captures");

  return 1;
}

// Test bishopGen: quen diagonal moves
int TestBishopGenQueen() {
  cout << "Testing bishopGen with queen moves..." << endl;

  Move move_list[256] = {0};
  Move* moves = move_list;

  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   | p | p | p |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   | P |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   | P |   | Q |
  // +---+---+---+---+---+---+---+---+

  Board board("8/8/8/8/8/5ppp/5P2/5P1Q", "w", "");
  magics::populateBishopTable(H1);
  board.bishopGen(&moves, 0, true);
  Move expected_moves[256] = {0};
  expected_moves[0] = moves::make(H1, G2, QUEEN, NONE, NONE, NONE, REGULAR_MOVE);
  expected_moves[1] = moves::make(H1, F3, QUEEN, PAWN, NONE, NONE, REGULAR_MOVE);
  sort(begin(move_list), end(move_list));
  sort(begin(expected_moves), end(expected_moves));
  ASSERT(!memcmp(move_list, expected_moves, sizeof(move_list[0]) * 256),
         "Incorrect queen moves");

  return 1;
}

// Test bishopGenPinned: bishops that are pinned
int TestBishopGenPinned() {
  cout << "Testing bishopGenPinned..." << endl;

  Move move_list[256] = {0};
  Move* moves = move_list;

  // +---+---+---+---+---+---+---+---+
  // | b |   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   | B |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |[K]|   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+

  Board board("b7/8/2B5/8/4K3/8/8/8", "w", "");
  magics::populateBishopTable(C6);
  masks::GenerateOpposite();
  masks::GenerateInterceding();
  board.bishopGenPinned(&moves, exp_2(C6), E4, true);
  Move expected_moves[256] = {0};
  expected_moves[0] =
      moves::make(C6, A8, BISHOP, BISHOP, NONE, NONE, REGULAR_MOVE);
  expected_moves[1] =
      moves::make(C6, B7, BISHOP, NONE, NONE, NONE, REGULAR_MOVE);
  expected_moves[2] =
      moves::make(C6, D5, BISHOP, NONE, NONE, NONE, REGULAR_MOVE);
  sort(begin(move_list), end(move_list));
  sort(begin(expected_moves), end(expected_moves));
  ASSERT(!memcmp(move_list, expected_moves, sizeof(move_list[0]) * 256),
         "Incorrect bishop pinned moves");

  return 1;
}

// Test rookGen: all rook moves
int TestRookGen() {
  cout << "Testing rookGen with quiet moves..." << endl;

  Move move_list[256] = {0};
  Move* moves = move_list;

  // +---+---+---+---+---+---+---+---+
  // |   | p |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // | P | R |   |   | p |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   | P |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+

  Board board("1p6/PR2p3/8/1P6/8/8/8/8", "w", "");
  magics::populateRookTable(B7);
  board.rookGen(&moves, 0, true);
  Move expected_moves[256] = {0};
  expected_moves[0] = moves::make(B7, B8, ROOK, PAWN, NONE, NONE, REGULAR_MOVE);
  expected_moves[1] = moves::make(B7, C7, ROOK, NONE, NONE, NONE, REGULAR_MOVE);
  expected_moves[2] = moves::make(B7, D7, ROOK, NONE, NONE, NONE, REGULAR_MOVE);
  expected_moves[3] = moves::make(B7, E7, ROOK, PAWN, NONE, NONE, REGULAR_MOVE);
  expected_moves[4] = moves::make(B7, B6, ROOK, NONE, NONE, NONE, REGULAR_MOVE);
  sort(begin(move_list), end(move_list));
  sort(begin(expected_moves), end(expected_moves));
  ASSERT(!memcmp(move_list, expected_moves, sizeof(move_list[0]) * 256),
         "Incorrect rook moves");

  return 1;
}

// Test rookGen: rook capture moves
int TestRookGenCaps() {
  cout << "Testing rookGen with capture moves..." << endl;

  Move cap_list[256] = {0};
  Move* caps = cap_list;

  // +---+---+---+---+---+---+---+---+
  // |   | p |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // | P | R |   |   | p |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   | P |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+

  Board board("1p6/PR2p3/8/1P6/8/8/8/8", "w", "");
  magics::populateRookTable(B7);
  board.rookGen(&caps, 0, false);
  Move expected_caps[256] = {0};
  expected_caps[0] = moves::make(B7, B8, ROOK, PAWN, NONE, NONE, REGULAR_MOVE);
  expected_caps[1] = moves::make(B7, E7, ROOK, PAWN, NONE, NONE, REGULAR_MOVE);
  sort(begin(cap_list), end(cap_list));
  sort(begin(expected_caps), end(expected_caps));
  ASSERT(!memcmp(cap_list, expected_caps, sizeof(cap_list[0]) * 256),
         "Incorrect rook moves");

  return 1;
}

// Test rookGen: queen horizontal moves
int TestRookGenQueen() {
  cout << "Testing rookGen with queen moves..." << endl;

  Move move_list[256] = {0};
  Move* moves = move_list;

  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   |   | P |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   |   | Q |
  // +---+---+---+---+---+---+---+---+

  Board board("8/8/8/8/8/8/7P/7Q", "w", "");
  magics::populateRookTable(H1);
  board.rookGen(&moves, 0, true);
  Move expected_moves[256] = {0};
  expected_moves[0] = moves::make(H1, G1, QUEEN, NONE, NONE, NONE, REGULAR_MOVE);
  expected_moves[1] = moves::make(H1, F1, QUEEN, NONE, NONE, NONE, REGULAR_MOVE);
  expected_moves[2] = moves::make(H1, E1, QUEEN, NONE, NONE, NONE, REGULAR_MOVE);
  expected_moves[3] = moves::make(H1, D1, QUEEN, NONE, NONE, NONE, REGULAR_MOVE);
  expected_moves[4] = moves::make(H1, C1, QUEEN, NONE, NONE, NONE, REGULAR_MOVE);
  expected_moves[5] = moves::make(H1, B1, QUEEN, NONE, NONE, NONE, REGULAR_MOVE);
  expected_moves[6] = moves::make(H1, A1, QUEEN, NONE, NONE, NONE, REGULAR_MOVE);
  sort(begin(move_list), end(move_list));
  sort(begin(expected_moves), end(expected_moves));
  ASSERT(!memcmp(move_list, expected_moves, sizeof(move_list[0]) * 256),
         "Incorrect horizontal queen moves");

  return 1;
}

// Test rookGenPinned: bishops that are pinned
int TestRookGenPinned() {
  cout << "Testing rookGenPinned..." << endl;

  Move move_list[256] = {0};
  Move* moves = move_list;

  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // | r |   | R |   |[K]|   |   |   |
  // +---+---+---+---+---+---+---+---+

  Board board("8/8/8/8/8/8/8/r1R1K3", "w", "");
  magics::populateRookTable(C1);
  masks::GenerateOpposite();
  masks::GenerateInterceding();
  board.rookGenPinned(&moves, exp_2(C1), E1, true);
  Move expected_moves[256] = {0};
  expected_moves[0] = moves::make(C1, A1, ROOK, ROOK, NONE, NONE, REGULAR_MOVE);
  expected_moves[1] = moves::make(C1, B1, ROOK, NONE, NONE, NONE, REGULAR_MOVE);
  expected_moves[2] = moves::make(C1, D1, ROOK, NONE, NONE, NONE, REGULAR_MOVE);
  sort(begin(move_list), end(move_list));
  sort(begin(expected_moves), end(expected_moves));
  ASSERT(!memcmp(move_list, expected_moves, sizeof(move_list[0]) * 256),
         "Incorrect pinned rook moves");

  return 1;
}

// Test bishopPins for bishops and queens pinning pieces diagonally
int TestBishopPins() {
  cout << "Testing bishopPins..." << endl;

  // +---+---+---+---+---+---+---+---+
  // | q |   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   | b |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   | P |   | P |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |[K]|   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   | P |   | P |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+

  Board board("q7/5b2/2P1P3/3K4/2P1P3/8/8/8", "w", "");
  magics::populateBishopTables();
  Bitboard pins = board.bishopPins(D5);
  Bitboard expectedPins = exp_2(C6) | exp_2(E6);
  ASSERT(pins == expectedPins, "Wrong bishop pins");

  return 1;
}

// Test rookPins for rooks and queens pinning pieces horizontally/vertically
int TestRookPins() {
  cout << "Testing rookPins..." << endl;

  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   | q |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   | P |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // | r |   | P |   |[K]| P |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+

  Board board("4q3/8/8/4P3/r1P1KP2/8/8/8", "w", "");
  magics::populateRookTables();
  Bitboard pins = board.rookPins(E4);
  Bitboard expectedPins = exp_2(C4) | exp_2(E5);
  ASSERT(pins == expectedPins, "Wrong rook pins");

  return 1;
}

// Tests attackSetGen on a board with one of each piece type.
int TestAttackSetGen() {
  cout << "Testing attackSetGen..." << endl;

  // +---+---+---+---+---+---+---+---+
  // |   |   |   |[k]|   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   | P |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   | p | b |
  // +---+---+---+---+---+---+---+---+
  // | r | n |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   | q |   |   |
  // +---+---+---+---+---+---+---+---+
  // | p |   |   |   |   | P |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+

  Board board("3k4/6P1/6pb/rn6/8/5q2/p4P2/8", "w", "");
  magics::populateBishopTables();
  magics::populateRookTables();
  Bitboard attacks = board.attackSetGen(BLACK);
  Bitboard expected_attacks =
      0b1010110011111110101101000101011110011110111110111001111001110001;
  ASSERT(attacks == expected_attacks, "Wrong attack set");

  return 1;
}

// Tests kingGen with quiet moves, captures and castling.
int TestKingMoves() {
  cout << "Testing kingGen with quiet moves and castling..." << endl;

  Move move_list[256] = {0};
  Move* moves = move_list;

  // +---+---+---+---+---+---+---+---+
  // |[k]|   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // | R |   |   |   |[K]|   |   | R |
  // +---+---+---+---+---+---+---+---+

  Board board("k7/8/8/8/8/8/8/R3K2R");
  Bitboard enemy_attacks = board.attackSetGen(BLACK);
  board.kingGen(&moves, E1, enemy_attacks, true);
  Move expected_moves[256] = {0};
  expected_moves[0] = moves::make(E1, D1, KING, NONE, NONE, NONE, KING_MOVE);
  expected_moves[1] = moves::make(E1, D2, KING, NONE, NONE, NONE, KING_MOVE);
  expected_moves[2] = moves::make(E1, E2, KING, NONE, NONE, NONE, KING_MOVE);
  expected_moves[3] = moves::make(E1, F2, KING, NONE, NONE, NONE, KING_MOVE);
  expected_moves[4] = moves::make(E1, F1, KING, NONE, NONE, NONE, KING_MOVE);
  expected_moves[5] = moves::make(E1, G1, KING, NONE, NONE, NONE, CASTLE);
  expected_moves[6] = moves::make(E1, C1, KING, NONE, NONE, NONE, CASTLE);
  sort(begin(move_list), end(move_list));
  sort(begin(expected_moves), end(expected_moves));
  ASSERT(!memcmp(move_list, expected_moves, sizeof(move_list[0]) * 256),
         "Incorrect king quiet moves");

  return 1;
}

// Tests kingGen with captures only.
int TestKingMovesCaps() {
  cout << "Testing kingGen capatures only..." << endl;

  Move cap_list[256] = {0};
  Move* caps = cap_list;

  // +---+---+---+---+---+---+---+---+
  // |[k]|   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   | p |   |   |
  // +---+---+---+---+---+---+---+---+
  // | R |   |   |   |[K]|   |   | R |
  // +---+---+---+---+---+---+---+---+

  Board board("k7/8/8/8/8/8/5p2/R3K2R");
  Bitboard enemy_attacks = board.attackSetGen(BLACK);
  board.kingGen(&caps, E1, enemy_attacks, false);
  Move expected_moves[256] = {0};
  expected_moves[3] = moves::make(E1, F2, KING, PAWN, NONE, NONE, KING_MOVE);
  sort(begin(cap_list), end(cap_list));
  sort(begin(expected_moves), end(expected_moves));
  ASSERT(!memcmp(cap_list, expected_moves, sizeof(cap_list[0]) * 256),
         "Incorrect king capture moves");

  return 1;
}

// Tests kingGen with quiet moves but no castling rights.
int TestKingMovesNoCastling() {
  cout << "Testing kingGen with quiet moves and no castling..." << endl;

  Move move_list[256] = {0};
  Move* moves = move_list;

  // +---+---+---+---+---+---+---+---+
  // |[k]|   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // | R |   |   |   |[K]|   |   | R |
  // +---+---+---+---+---+---+---+---+

  Board board("k7/8/8/8/8/8/8/R3K2R", "w", "kq");
  Bitboard enemy_attacks = board.attackSetGen(BLACK);
  board.kingGen(&moves, E1, enemy_attacks, true);
  Move expected_moves[256] = {0};
  expected_moves[0] = moves::make(E1, D1, KING, NONE, NONE, NONE, KING_MOVE);
  expected_moves[1] = moves::make(E1, D2, KING, NONE, NONE, NONE, KING_MOVE);
  expected_moves[2] = moves::make(E1, E2, KING, NONE, NONE, NONE, KING_MOVE);
  expected_moves[3] = moves::make(E1, F2, KING, NONE, NONE, NONE, KING_MOVE);
  expected_moves[4] = moves::make(E1, F1, KING, NONE, NONE, NONE, KING_MOVE);
  sort(begin(move_list), end(move_list));
  sort(begin(expected_moves), end(expected_moves));
  ASSERT(!memcmp(move_list, expected_moves, sizeof(move_list[0]) * 256),
         "Incorrect king quiet non-castling moves");

  return 1;
}

// Tests move generation to a given square
int TestGenerateMovesTo() {
  cout << "Testing GenerateMovesTo on empty square..." << endl;

  Move move_list[256] = {0};
  Move* moves = move_list;

  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   | R |   | Q |   |
  // +---+---+---+---+---+---+---+---+
  // | k |   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   | : |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   | P | p |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   | N | B |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |[K]|   |   |   |
  // +---+---+---+---+---+---+---+---+

  Board board("4R1Q1/k7/8/3Pp3/5NB1/8/8/4K3", "w", "", "e6");
  magics::populateBishopTables();
  magics::populateRookTables();
  board.generateMovesTo(&moves, E6, NONE, 0, 0);
  Move expected_moves[256] = {0};
  expected_moves[0] =
      moves::make(D5, E6, PAWN, PAWN, NONE, NONE, EN_PASSANT_CAP);
  expected_moves[1] =
      moves::make(F4, E6, KNIGHT, NONE, NONE, NONE, REGULAR_MOVE);
  expected_moves[2] = moves::make(E8, E6, ROOK, NONE, NONE, NONE, REGULAR_MOVE);
  expected_moves[3] = moves::make(G8, E6, QUEEN, NONE, NONE, NONE, REGULAR_MOVE);
  expected_moves[4] =
      moves::make(G4, E6, BISHOP, NONE, NONE, NONE, REGULAR_MOVE);
  sort(begin(move_list), end(move_list));
  sort(begin(expected_moves), end(expected_moves));
  ASSERT(!memcmp(move_list, expected_moves, sizeof(move_list[0]) * 256),
         "Incorrect quiet movesTo moves");

  return 1;
}

// Tests capture generation to a given square
int TestGenerateMovesToCaps() {
  cout << "Testing generateMovesTo on an occupied square..." << endl;

  Move move_list[256] = {0};
  Move* moves = move_list;

  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   | N |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   | B |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   | r | R |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   | P |[K]|   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+

  Board board("4N3/6B1/5rR1/4PK2/8/8/8/8", "w", "");
  magics::populateBishopTables();
  magics::populateRookTables();
  board.generateMovesTo(&moves, F6, ROOK, 0, 0);
  Move expected_moves[256] = {0};
  expected_moves[0] = moves::make(E5, F6, PAWN, ROOK, NONE, NONE, REGULAR_MOVE);
  expected_moves[1] =
      moves::make(E8, F6, KNIGHT, ROOK, NONE, NONE, REGULAR_MOVE);
  expected_moves[2] = moves::make(G6, F6, ROOK, ROOK, NONE, NONE, REGULAR_MOVE);
  expected_moves[3] =
      moves::make(G7, F6, BISHOP, ROOK, NONE, NONE, REGULAR_MOVE);
  sort(begin(move_list), end(move_list));
  sort(begin(expected_moves), end(expected_moves));
  ASSERT(!memcmp(move_list, expected_moves, sizeof(move_list[0]) * 256),
         "Incorrect capture movesTo moves");

  return 1;
}

// Tests evasion generation
int TestEvasionGen() {
  cout << "Testing evasionGen..." << endl;

  Move move_list[256] = {0};
  Move* moves = move_list;

  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   | Q |   | q |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   | B |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   | r |   | : |   |[K]|   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   | p | P |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   | N |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // | k |   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+

  Board board("5Q1q/4B3/2r3K1/4pP2/6N1/8/8/k7", "w", "", "E6");
  magics::populateBishopTables();
  magics::populateRookTables();
  masks::GenerateOpposite();
  masks::GenerateInterceding();
  Bitboard enemy_attacks = board.attackSetGen(BLACK);
  board.evasionGen(&moves, enemy_attacks, 0, G6);
  Move expected_moves[256] = {0};
  expected_moves[0] =
      moves::make(E7, F6, BISHOP, NONE, NONE, NONE, REGULAR_MOVE);
  expected_moves[1] =
      moves::make(E7, D6, BISHOP, NONE, NONE, NONE, REGULAR_MOVE);
  expected_moves[2] =
      moves::make(F5, E6, PAWN, PAWN, NONE, NONE, EN_PASSANT_CAP);
  expected_moves[3] = moves::make(F5, F6, PAWN, NONE, NONE, NONE, REGULAR_MOVE);
  expected_moves[4] =
      moves::make(G4, F6, KNIGHT, NONE, NONE, NONE, REGULAR_MOVE);
  expected_moves[5] =
      moves::make(F8, F6, QUEEN, NONE, NONE, NONE, REGULAR_MOVE);
  expected_moves[6] = moves::make(G6, F7, KING, NONE, NONE, NONE, KING_MOVE);
  expected_moves[7] = moves::make(G6, G5, KING, NONE, NONE, NONE, KING_MOVE);
  sort(begin(move_list), end(move_list));
  sort(begin(expected_moves), end(expected_moves));
  ASSERT(!memcmp(move_list, expected_moves, sizeof(move_list[0]) * 256),
         "Incorrect evasions");

  return 1;
}

int main() {
  int t = 0;

  t += TestPawnGen();
  t += TestPawnGenPromotions();
  t += TestPawnCaps();
  t += TestPawnCapsEnPassant();
  t += TestPawnGenPinned();
  t += TestKnightGen();
  t += TestKnightCaps();
  t += TestBishopGen();
  t += TestBishopGenCaps();
  t += TestBishopGenQueen();
  t += TestBishopGenPinned();
  t += TestRookGen();
  t += TestRookGenCaps();
  t += TestRookGenQueen();
  t += TestRookGenPinned();
  t += TestBishopPins();
  t += TestRookPins();
  t += TestAttackSetGen();
  t += TestKingMoves();
  t += TestKingMovesCaps();
  t += TestKingMovesNoCastling();
  t += TestGenerateMovesTo();
  t += TestGenerateMovesToCaps();
  t += TestEvasionGen();

  cout << endl;
  cout << t << " test(s) OK" << endl;
  return 0;
}
