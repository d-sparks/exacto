#ifndef exacto_src_moves_h
#define exacto_src_moves_h

#include <stdint.h>
#include <string>

#include "bitboard.h"
#include "defines.h"

// Moves are stored as bitstrings. The bits represent (LSB on right):

// [00000000000000000000000000111111]: source square (6 bits)
// [00000000000000000000111111000000]: dest square (6 bits)
// [00000000000000000111000000000000]: attacker (3 bits)
// [00000000000000111000000000000000]: defender (3 bits)
// [00000000001111000000000000000000]: en passant column (4 bits)
// [00000011110000000000000000000000]: castling data (4 bits - KQkq)
// [11111100000000000000000000000000]: special data (6 bits)

namespace exacto {

typedef uint32_t Move;

namespace moves {

ind source(Move m);

ind dest(Move m);

ind attacker(Move m);

ind defender(Move m);

ind en_passant(Move m);

ind castling(Move m);

ind special(Move m);

Move make(Move source,
          Move dest,
          Move attacker,
          Move defender,
          Move en_passant,
          Move castling,
          Move special);

// Returns the algebraic notation for the given move. (E.g. e2e4.)
std::string algebraic(Move m);

// Castling is encoded into four bits in a move in the 22-25 indices,
// representing KQkq respectively.
Move EncodeCastling(Bitboard castling_data);

// Mutates the given move by setting the relevant bit indicating castling rights
// change.
void EncodeKingsideCastlingChange(Move* move, bool color);

// Mutates the given move by setting the relevant bit indicating castling rights
// change.
void EncodeQueensideCastlingChange(Move* move, bool color);

// Decodes a castlingCode as outputted by EncodeCastling.
Bitboard DecodeCastling(ind castlingCode);

// Print a move
void print(Move move);

// Create a Move from cin input
bool ReadMoveFromStdin(Move* candidateMoves, Move* output);

}  // namespace squares

}  // namespace exacto

#endif  // exacto_src_moves_h
