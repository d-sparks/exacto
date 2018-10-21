#ifndef exacto_src_magics_h
#define exacto_src_magics_h

#include <vector>

#include "bitboard.h"
#include "defines.h"

namespace exacto {

namespace magics {

// 9 bit bishop magics (generated with exacto 0.e)
extern Bitboard BISHOP_MOVES[64][EXP2_BISHOP_MAGIC_BITS];
extern const Bitboard BISHOP_MAGICS[64];

// 11 bit rook magics (generated with exaxcto 0.e)
extern Bitboard ROOK_MOVES[64][EXP2_ROOK_MAGIC_BITS];
extern Bitboard const ROOK_MAGICS[64];

// The magic formula, helper for bishops
Bitboard hashBishop(Bitboard bb, ind square);

// The magic formula, helper for rooks
Bitboard hashRook(Bitboard bb, ind square);

// bishopMoves gives the moves for a bishop given an occupancy Bitboard and a
// square
Bitboard bishopMoves(ind square, Bitboard occupancy);

// rookMoves gives the moves for a bishop given an occupancy Bitboard and a
// square.
Bitboard rookMoves(ind square, Bitboard occupancy);

// generateSubsets finds all sub-bitstrings of a given bitstring. It is used to
// generate occupancy Bitboards for magics. It works by going through indices x
// = 0, ..., 2^(popcount(b)) - 1, associating each such x to a sub-bitstring of
// b by shifting the bits of x to the indexes of the nonzero bits of b.
void generateSubsets(Bitboard b, std::vector<Bitboard>* subsets);

// Generate the moves for a given occupancy Bitboard, square and set of
// directions.
Bitboard generateMovesFromOccupancy(ind square,
                                    Bitboard occupancy,
                                    Bitboard mask,
                                    int directionDeltas[4]);

// Populate the bishop move table for a certain square and a certain number of
// bits.
void populateBishopTable(ind square);

// Populate the rook move table for a certain square and a certain number of
// bits.
void populateRookTable(ind square);

// Generate bishop move table for all squares.
void populateBishopTables();

// Generate rook move table for all squares.
void populateRookTables();

}  // namespace magics

}  // namespace exacto

#endif  // exacto_src_magics_h
