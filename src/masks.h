#ifndef exacto_src_masks_h
#define exacto_src_masks_h

#include "bitboard.h"

namespace exacto {

namespace masks {

bool WrapIter(int *square, int delta);

// Initialize dynamically set mask tables.
void init();

Bitboard promo_rank(bool color);

void GenerateInterceding();  // TODO: don't need to expose?

void GenerateOpposite();  // TODO: don't need to expose?

void GeneratePawnChecks();  // TODO: don't need to expose?

extern const Bitboard RANK[8];

extern const Bitboard FILE[8];

extern const Bitboard KNIGHT_MOVES[64];

extern const Bitboard KING_MOVES[64];

extern const Bitboard BISHOP_MASKS[64];

extern Bitboard const ROOK_MASKS[64];

extern Bitboard INTERCEDING[64][64];

extern Bitboard PAWN_CHECKS[2][64];

extern Bitboard OPPOSITE[64][64];

}  // namespace masks

}  // namespace exacto

#endif  // exacto_src_masks_h
