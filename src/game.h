#ifndef exacto_src_game_h
#define exacto_src_game_h

#include <string>

#include "board.h"
#include "moves.h"

namespace exacto {

class Game : public Board {
 public:
  Game(const std::string& brd = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR",
       const std::string& clr = "w",
       const std::string& cstl = "KQkq",
       const std::string& ep = "-",
       const std::string& hm = "0",
       const std::string& fm = "1");

  void SetGame(
      const std::string& brd = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR",
      const std::string& clr = "w",
      const std::string& cstl = "KQkq",
      const std::string& ep = "-",
      const std::string& hm = "0",
      const std::string& fm = "1");

  void MakeMove(Move* m);
  void UnmakeMove(Move m);
  void MakeNull(Move* m);
  void UnmakeNull(Move m);
  bool operator==(const Game& other) const;

  int full_move_number() { return (move_number / 2) + 1; };
  int move_number;
  ind repitition_ub() {
    return repitition_hash[hash_key >> REPITITION_HASH_SHIFT];
  }

  // These will have garbage beyond move_number index, which is accoutned for in
  // operator==.
  ind half_moves() { return half_move_history[move_number]; }
  ind half_move_history[STACK_SIZE];
  Bitboard position_history[STACK_SIZE];  // zobrist keys
  ind repitition_hash[REPITITION_HASH_SIZE];

  std::string fancy_algebraic(Move move);
};

// TODO: move these somewhere? maybe Exacto?
Bitboard perft(Game *game, int depth);
Bitboard divide(Game *game, int depth);

}  // namespace exacto

#endif  // exacto_src_game_h
