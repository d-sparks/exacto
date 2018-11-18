#define _TEST 1

#include "../src/game.h"

#include <cstring>
#include <map>
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

int TestFancyAlgebraic() {
  cout << "Testing fancy algebraic notation..." << endl;

  Move move_list[256];

  // +---+---+---+---+---+---+---+---+
  // |   |   | n |   |[k]|   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   | P |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   |   | : |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   | P | p | P |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   | N |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // |   |   |   |   | N |   |   | Q |
  // +---+---+---+---+---+---+---+---+
  // | p |   |   |   |   |   |   |   |
  // +---+---+---+---+---+---+---+---+
  // | R |   |   |   |[K]|   |   | R |
  // +---+---+---+---+---+---+---+---+

  Game game("2n1k3/1P6/8/4PpP1/3N4/4N2Q/p7/R3K2R", "w", "KQ", "f6");
  game.MoveGen(move_list);

  map<string, string> algebraic_to_fancy{
      {"a1b1", "Rb1"}, {"a1c1", "Rc1"}, {"a1d1", "Rd1"}, {"e1c1", "O-O-O"},
      {"e1d1", "Kd1"}, {"e1d2", "Kd2"}, {"e1e2", "Ke2"}, {"e1f2", "Kf2"},
      {"e1f1", "Kf1"}, {"e1g1", "O-O"}, {"h1f1", "Rf1"}, {"h1g1", "Rg1"},
      {"h1h2", "Rh2"}, {"h3f1", "Qf1"}, {"h3g2", "Qg2"}, {"h3h2", "Qh2"},
      {"h3f3", "Qf3"}, {"h3g3", "Qg3"}, {"h3g4", "Qg4"}, {"h3f5", "Qxf5"},
      {"h3h4", "Qh4"}, {"h3h5", "Qh5+"}, {"h3h6", "Qh6"}, {"h3h7", "Qh7"},
      {"h3h8", "Qh8+"}, {"d4b5", "Nb5"}, {"d4c6", "Nc6"}, {"d4e6", "Ne6"},
      {"d4f5", "Nd4xf5"}, {"d4f3", "Nf3"}, {"d4e2", "Ne2"}, {"d4c2", "Nd4c2"},
      {"d4b3", "Nb3"}, {"e3c4", "Nc4"}, {"e3d5", "Nd5"}, {"e3f5", "Ne3xf5"},
      {"e3g4", "Ng4"}, {"e3g2", "Ng2"}, {"e3f1", "Nf1"}, {"e3d1", "Nd1"},
      {"e3c2", "Ne3c2"}, {"e5e6", "e6"}, {"e5f6", "exf6"}, {"g5g6", "g6"},
      {"g5f6", "gxf6"}, {"b7b8Q", "b8Q"}, {"b7b8R", "b8R"}, {"b7b8B", "b8B"},
      {"b7b8N", "b8N"}, {"b7c8Q", "bxc8Q+"}, {"b7c8R", "bxc8R+"},
      {"b7c8B", "bxc8B"}, {"b7c8N", "bxc8N"}, {"a1a2", "Rxa2"}};

  for (int i = 0; move_list[i]; ++i) {
    string actual = game.fancy_algebraic(move_list[i]);
    string expected = algebraic_to_fancy[moves::algebraic(move_list[i])];
    ASSERT_EQ(actual, expected, "Expected " + actual + " == " + expected);
  }

  return 1;
}

int TestMakeNullUnmakeNull() {
  cout << "Testing that MakeNull and UnmakeNull are inverses..." << endl;

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
  ASSERT_EQ(game1, game2, "operator== failed for game.");

  Move null_move = 0;
  game1.MakeNull(&null_move);
  ASSERT(!memcmp(game1.board, game2.board, sizeof(game1.board[0]) * 64),
         "Null move altered board state.");
  ASSERT(game1.wtm != game2.wtm, "Null move didn't change wtm.");
  ASSERT_EQ(game1.en_passant, 0, "En passant not removed.");
  game1.UnmakeNull(null_move);
  ASSERT_EQ(game1, game2, "UnmakeNull didn't undo MakeNull.");

  return 1;
}

int main() {
  magics::PopulateBishopTables();
  magics::PopulateRookTables();

  int t = 0;

  t += TestMakeMoveUnmakeMove();
  t += TestReset50MoveLogic();
  t += TestFancyAlgebraic();
  t += TestMakeNullUnmakeNull();

  cout << endl;
  cout << t << " test(s) OK" << endl;
  return 0;
}
