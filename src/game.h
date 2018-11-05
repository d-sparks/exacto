#ifndef exacto_src_game_h
#define exacto_src_game_h

#include <string>

#include "board.h"
#include "moves.h"

namespace exacto {

class Game : public Board {
  friend class Exacto;

 public:
  Game(std::string brd = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR",
       std::string clr = "w",
       std::string cstl = "KQkq",
       std::string ep = "-",
       std::string hm = "0",
       std::string fm = "0");
  ~Game();

  void MakeMove(Move* m);
  void UnmakeMove(Move m);
  bool operator==(const Game& other) const;

#ifndef _TEST
 private:
#endif
  int move_number;
  // These will have garbage beyond move_number index, which is accoutned for in
  // operator==.
  ind half_moves[STACK_SIZE];
  Bitboard position_history[STACK_SIZE];  // zobrist keys
  ind repitition_hash[REPITITION_HASH_SIZE];
};

// TODO: move these somewhere? maybe Exacto?
Bitboard perft(Game *game, int depth);
Bitboard divide(Game *game, int depth);

}  // namespace exacto

#endif  // exacto_src_game_h
