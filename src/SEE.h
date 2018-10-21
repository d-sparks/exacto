#ifndef exacto_src_SEE_h
#define exacto_src_SEE_h

#include "bitboard.h"
#include "board.h"

namespace exacto {

namespace SEE {

int16_t see(Board* game, Move move);

int16_t next(Board* board, int16_t previous_val, ind square);

void MakeMove(Board* board, Bitboard source, ind attacker);

void UnmakeMove(Board* board, Bitboard source, ind attacker);

ind least_valuable_attacker_square(Board* board, ind square);

}  // namespace SEE

}  // namespace exacto

#endif  // exacto_src_SEE_h
