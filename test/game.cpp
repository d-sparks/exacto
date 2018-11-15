#define _TEST 1

#include "../src/game.h"

#include <vector>

#include "../src/board.h"
#include "../src/defines.h"
#include "../src/magics.h"
#include "../src/masks.h"
#include "../src/moves.h"
#include "../src/squares.h"
#include "assert.h"

using namespace exacto;
using namespace std;

// Greatcomment
int TestMakeMoveUnmakeMove() {
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

  Game game1("r2qk3/1P6/8/2Pp4/4P3/1N3b2/5PPP/R3K2R", "w", "KQq", "D6");
  Game game2 = game1;
  ASSERT(game1 == game2, "operator== failed for game.");
  magics::PopulateBishopTables();
  magics::PopulateRookTables();
  Move move_list[256] = {0};
  game1.MoveGen(move_list, true);
  for (Move* move = move_list; *move; move++) {
    game1.MakeMove(move);
    game1.UnmakeMove(*move);
    ASSERT(game1 == game2,
           "UnmakeMove didn't undo MakeMove: " + moves::algebraic(*move));
  }

  return 1;
}

int TestReset50MoveLogic() {
  cout << "Testing that pawn/capture moves resets 50 move clock..." << endl;

  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |[k]|   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   | : |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   | P | p |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   | R |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |[K]|   |   |   |
  // +---+---+---+---+---+---+---+---+

  Game game("4k3/8/8/2Pp4/8/8/5r2/4K3", "w", "-", "D6", "20", "100");
  vector<Move> noisy_moves{
      moves::make(E1, F2, KING, ROOK, D6, 0, 0),
      moves::make(C5, D6, PAWN, PAWN, D6, 0, EN_PASSANT_CAP),
      moves::make(C5, C6, PAWN, NONE, D6, 0, 0)};
  vector<Move> quiet_moves{
      moves::make(E1, F1, KING, NONE, D6, 0, 0),
      moves::make(E1, E2, KING, NONE, D6, 0, 0),
      moves::make(E1, D2, KING, NONE, D6, 0, 0),
      moves::make(E1, D1, KING, NONE, D6, 0, 0)};

  for (Move move : noisy_moves) {
    game.MakeMove(&move);
    ASSERT(game.half_moves() == 0,
           "Didn't reset 50move clock: " + moves::algebraic(move));
    game.UnmakeMove(move);
  }
  for (Move move : quiet_moves) {
    game.MakeMove(&move);
    ASSERT(game.half_moves() == 21,
           "Didn't increment 50move clock: " + moves::algebraic(move));
    game.UnmakeMove(move);
  }

  return 1;
}

int main() {
  int t = 0;

  t += TestMakeMoveUnmakeMove();
  t += TestReset50MoveLogic();

  cout << endl;
  cout << t << " test(s) OK" << endl;
  return 0;
}
