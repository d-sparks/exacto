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
Bitboard HashBishop(Bitboard bb, ind square);

// The magic formula, helper for rooks
Bitboard HashRook(Bitboard bb, ind square);

// BishopMoves gives the moves for a bishop given an occupancy Bitboard and a
// square
Bitboard BishopMoves(ind square, Bitboard occupancy);

// RookMoves gives the moves for a bishop given an occupancy Bitboard and a
// square.
Bitboard RookMoves(ind square, Bitboard occupancy);

// GenerateSubsets finds all sub-bitstrings of a given bitstring. It is used to
// generate occupancy Bitboards for magics. It works by going through indices x
// = 0, ..., 2^(popcount(b)) - 1, associating each such x to a sub-bitstring of
// b by shifting the bits of x to the indexes of the nonzero bits of b.
void GenerateSubsets(Bitboard b, std::vector<Bitboard>* subsets);

// Generate the moves for a given occupancy Bitboard, square and set of
// directions.
Bitboard GenerateMovesFromOccupancy(ind square,
                                    Bitboard occupancy,
                                    Bitboard mask,
                                    int directionDeltas[4]);

// Populate the bishop move table for a certain square and a certain number of
// bits.
void PopulateBishopTable(ind square);

// Populate the rook move table for a certain square and a certain number of
// bits.
void PopulateRookTable(ind square);

// Generate bishop move table for all squares.
void PopulateBishopTables();

// Generate rook move table for all squares.
void PopulateRookTables();

}  // namespace magics

}  // namespace exacto

#endif  // exacto_src_magics_h
