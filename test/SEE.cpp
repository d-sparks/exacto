#include "../src/SEE.h"

#include "../src/board.h"
#include "../src/game.h"
#include "../src/inlines.h"
#include "../src/magics.h"
#include "../src/masks.h"
#include "../src/moves.h"
#include "../src/squares.h"
#include "assert.h"

using namespace exacto;
using namespace std;

// Greatcomment
int TestMakeMoveUnMakeMove() {
  cout << "Testing that MakeMove and UnmakeMove are inverses..." << endl;

  // +---+---+---+---+---+---+---+---+
  // | r |   |   | q |[k]|   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   | P |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   | : |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   | P | p |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   | P |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   | N |   |   |   | b |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   | P | P | P |
  // +---+---+---+---+---+---+---+---+
  // | R |   |   |   |[K]|   |   | R |
  // +---+---+---+---+---+---+---+---+

  Game game1("r2qk3/1P6/8/2Pp4/4P3/1N3b2/5PPP/R3K2R", "w", "KQq", "-");
  Game game2 = game1;
  Move move_list[256] = {0};
  game1.MoveGen(move_list, true);
  for (Move* move = move_list; *move; move++) {
    Bitboard source_bb = exp_2(moves::source(*move));
    ind attacker = moves::attacker(*move);
    SEE::MakeMove(&game1, source_bb, attacker);
    SEE::UnmakeMove(&game1, source_bb, attacker);
    ASSERT(game1 == game2,
           "UnmakeMove didn't undo MakeMove: " + moves::algebraic(*move));
  }

  return 1;
}

// Tests that SEE gives appropriate values
int TestSEEValues() {
  cout << "Testing that SEE gives the right values..." << endl;

  // +---+---+---+---+---+---+---+---+
  // |   |   |   | r |[k]| r |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   | n |   |   | P |   | B |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // | P |   | P |[K]|   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+

  pair<Move, int16_t> moves[5] = {
      {moves::make(C4, C5, PAWN, NONE, NONE, NONE, NONE), 0},
      {moves::make(A4, A5, PAWN, NONE, NONE, NONE, NONE), -PAWN_VAL},
      {moves::make(D4, C5, KING, NONE, NONE, NONE, NONE), -MATESCORE},
      {moves::make(E7, D8, PAWN, ROOK, NONE, NONE, PROMOTE_QUEEN),
       ROOK_VAL - PAWN_VAL},
      {moves::make(E7, F8, PAWN, ROOK, NONE, NONE, PROMOTE_QUEEN), ROOK_VAL}};

  Game game("3rkr2/1n2P1B1/8/8/P1PK4/8/8/8", "w", "-", "-");
  Game gameRef("3rkr2/1n2P1B1/8/8/P1PK4/8/8/8", "w", "-", "-");
  for (int i = 0; i < 5; i++) {
    Move move = moves[i].first;
    int16_t expectedScore = moves[i].second;
    int16_t score = SEE::see(&game, move);
    ASSERT(game == gameRef,
           "SEE corrupted board state for move " + moves::algebraic(move));
    ASSERT(score == expectedScore,
           "SEE miscalculated " + moves::algebraic(move));
  }

  return 1;
}

int main() {
  int t = 0;

  magics::populateBishopTables();
  magics::populateRookTables();
  masks::init();

  t += TestMakeMoveUnMakeMove();
  t += TestSEEValues();

  cout << endl;
  cout << t << " test(s) OK" << endl;
  return 0;
}
