#include "exacto.h"

#include "game.h"
#include "inlines.h"

namespace exacto {

int16_t Exacto::Evaluate(Game* game) {
  int16_t score = 0;

  for (ind color = BLACK; color <= WHITE; ++color) {
    for (ind piece = PAWN; piece <= KING; ++piece) {
      if (piece == ROOK || piece == QUEEN) {
        continue;
      }
      Bitboard pieceboard = game->pieces[color][piece];
      for (ind sq = bitscan(pieceboard); pieceboard; sq = bitscan(pieceboard)) {
        score += PVT[color][piece][sq];
        pieceboard ^= exp_2(sq);
      }
    }
  }

  score += popcount(game->pieces[WHITE][ROOK]) * ROOK_VAL;
  score += popcount(game->pieces[WHITE][QUEEN]) * QUEEN_VAL;
  score -= popcount(game->pieces[BLACK][ROOK]) * ROOK_VAL;
  score -= popcount(game->pieces[BLACK][QUEEN]) * QUEEN_VAL;

  return game->wtm ? score : -score;
}

}  // namespace exacto
