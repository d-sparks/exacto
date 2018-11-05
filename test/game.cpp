#define _TEST 1

#include "../src/game.h"

#include "../src/board.h"
#include "../src/magics.h"
#include "../src/masks.h"
#include "../src/moves.h"
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
  magics::populateBishopTables();
  magics::populateRookTables();
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

int main() {
  int t = 0;

  t += TestMakeMoveUnmakeMove();

  cout << endl;
  cout << t << " test(s) OK" << endl;
  return 0;
}
