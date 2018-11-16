#include "../src/exacto.h"

#include <iostream>

#include "../src/board.h"
#include "../src/game.h"
#include "../src/hash.h"
#include "../src/magics.h"
#include "../src/masks.h"
#include "../src/moves.h"
#include "../src/SEE.h"
#include "../src/squares.h"
#include "assert.h"

using namespace exacto;
using namespace std;

// Tests that moves are sorted appropriately
int TestSortMoves() {
  cout << "Testing that sort moves sorts correctly..." << endl;

  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |[k]| r |   | q |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   | B |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   |   | B |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   | P |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   | P |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   | p |   |   | p | p |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   | n |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   | b | R |[K]|
  // +---+---+---+---+---+---+---+---+

  Game game("4kr1q/6B1/7B/6P1/6P1/2p2pp1/6n1/5bRK", "w", "-", "-");
  Game gameRef("4kr1q/6B1/7B/6P1/6P1/2p2pp1/6n1/5bRK", "w", "-", "-");
  Exacto exacto;
  Move move_list[256] = {0};
  game.MoveGen(move_list, true);
  pair<Move, int16_t> expected[9] = {
      {moves::make(G7, D4, BISHOP, NONE, NONE, NONE, NONE), MATESCORE},
      {moves::make(G7, H8, BISHOP, QUEEN, NONE, NONE, NONE),
       QUEEN_VAL - BISHOP_VAL},
      {moves::make(G7, F8, BISHOP, ROOK, NONE, NONE, NONE), ROOK_VAL},
      {moves::make(G1, F1, ROOK, BISHOP, NONE, NONE, NONE), BISHOP_VAL},
      {moves::make(G5, G6, PAWN, NONE, NONE, NONE, NONE), 0},
      {moves::make(G7, F6, BISHOP, NONE, NONE, NONE, NONE), 0},
      {moves::make(G7, C3, BISHOP, PAWN, NONE, NONE, NONE),
       PAWN_VAL - BISHOP_VAL},
      {moves::make(G1, G2, ROOK, KNIGHT, NONE, NONE, NONE),
       KNIGHT_VAL - ROOK_VAL},
      {moves::make(G7, E5, BISHOP, NONE, NONE, NONE, NONE), -BISHOP_VAL}};
  exacto.hash.record(game.hash_key, expected[0].first, 1, HASH_EXACT, 0);
  exacto.SortMoves(&game, move_list);
  ASSERT(game == gameRef, "Corrupted position");
  for (int i = 0; i < 8; i++) {
    cout << "..." << moves::algebraic(move_list[i]) << " should be "
         << moves::algebraic(expected[i].first) << endl;
    ASSERT(move_list[i] == expected[i].first, "Bad move sort");
  }

  return 1;
}

// Tests draw by threefold repitition and 50 move rule.
int TestDrawThreefold() {
  cout << "Testing that sort moves sorts correctly..." << endl;

  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |[k]|   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   | r |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   | R |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |[K]|   |   |   |
  // +---+---+---+---+---+---+---+---+

  Game game("4kr1q/6B1/7B/6P1/6P1/2p2pp1/6n1/5bRK", "w", "-", "-");
  vector<Move> repeat{
    moves::make(C2, B2, ROOK, NONE, NONE, NONE, NONE),
    moves::make(F7, G7, ROOK, NONE, NONE, NONE, NONE),
    moves::make(B2, C2, ROOK, NONE, NONE, NONE, NONE),
    moves::make(G7, F7, ROOK, NONE, NONE, NONE, NONE)};
  Exacto exacto;
  for (int i = 0; i < 2; ++i) {
    ASSERT(!exacto.drawn_by_repitition_or_50_move_rule(&game),
           "Should not be drawn by repitition.");
    for (Move move : repeat) {
      game.MakeMove(&move);
    }
  }
  ASSERT(exacto.drawn_by_repitition_or_50_move_rule(&game),
         "Should be drawn by repitition.");

  return 1;
}

int main() {
  int t = 0;

  magics::PopulateBishopTables();
  magics::PopulateRookTables();
  masks::init();

  t += TestSortMoves();
  t += TestDrawThreefold();

  cout << endl;
  cout << t << " test(s) OK" << endl;
  return 0;
}
