#ifndef exacto_src_game_h
#define exacto_src_game_h

#include <string>

#include "board.h"
#include "moves.h"

namespace exacto {

class Game : public Board {
  friend class Exacto;

 public:
  Game(const std::string& brd = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR",
       const std::string& clr = "w",
       const std::string& cstl = "KQkq",
       const std::string& ep = "-",
       const std::string& hm = "0",
       const std::string& fm = "0");

  void MakeMove(Move* m);
  void UnmakeMove(Move m);
  bool operator==(const Game& other) const;

  int move_number;

#ifndef _TEST
 private:
#endif
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
