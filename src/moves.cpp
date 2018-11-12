#include "moves.h"

#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "bitboard.h"
#include "game.h"
#include "inlines.h"
#include "squares.h"

namespace exacto {

namespace moves {

ind source(Move m) { return m & 63; }

ind dest(Move m) { return (m >> 6) & 63; }

ind attacker(Move m) { return (m >> 12) & 7; }

ind defender(Move m) { return (m >> 15) & 7; }

ind en_passant(Move m) { return (m >> 18) & 15; }

ind castling(Move m) { return (m >> 22) & 15; }

ind special(Move m) { return (m >> 26); }

Move make(Move source,
          Move dest,
          Move attacker,
          Move defender,
          Move en_passant,
          Move castling,
          Move special) {
  return source | (dest << 6) | (attacker << 12) | (defender << 15) |
         (en_passant << 18) | (castling << 22) | (special << 26);
}

std::string algebraic(Move m) {
  ind promot_data = special(m);
  std::string out = squares::algebraic[source(m)] + squares::algebraic[dest(m)];
  if (promot_data == PROMOTE_BISHOP) out += "B";
  if (promot_data == PROMOTE_KNIGHT) out += "N";
  if (promot_data == PROMOTE_ROOK) out += "R";
  if (promot_data == PROMOTE_QUEEN) out += "Q";
  return out;
}

// Castling is encoded into four bits in a move in the 22-25 indices,
// representing KQkq respectively.
Move EncodeCastling(Bitboard castling_data) {
  Move out = 0;
  out |= (exp_2(G1) & castling_data) << (22 - G1);
  out |= (exp_2(C1) & castling_data) << (23 - C1);
  out |= (exp_2(G8) & castling_data) >> (G8 - 24);
  out |= (exp_2(C8) & castling_data) >> (C8 - 25);
  return out;
}

// Mutates the given move by setting the relevant bit indicating castling rights
// change.
void EncodeKingsideCastlingChange(Move* move, bool color) {
  *move |= Move(1) << (24 - 2 * color);
}

// Mutates the given move by setting the relevant bit indicating castling rights
// change.
void EncodeQueensideCastlingChange(Move* move, bool color) {
  *move |= Move(1) << (25 - 2 * color);
}

// Decodes a castling_code as outputted by EncodeCastling.
Bitboard DecodeCastling(ind castling_code) {
  Bitboard out = 0;
  out |= (Bitboard)(1 & castling_code) << (G1 - 0);
  out |= (Bitboard)(2 & castling_code) << (C1 - 1);
  out |= (Bitboard)(4 & castling_code) << (G8 - 2);
  out |= (Bitboard)(8 & castling_code) << (C8 - 3);
  return out;
}

// Print a move
void print(Move move) { std::cout << algebraic(move); }

// Create a Move from cin input
bool ReadMoveFromStdin(Move* candidate_moves, Move* output) {
  std::string move_string;
  std::cin >> move_string;
  std::string source_string = move_string.substr(0, 2);
  std::string dest_string = move_string.substr(2, 2);
  ind source_ind = squares::index(source_string);
  ind dest_ind = squares::index(dest_string);

  std::vector<Move> matches;

  for (Move* move = candidate_moves; *move; move++) {
    if (source_ind == source(*move) && dest_ind == dest(*move)) {
      matches.push_back(*move);
    }
  }

  if (matches.size() != 1) {
    std::map<char, Move> promo_type{{'Q', PROMOTE_QUEEN}, {'R', PROMOTE_ROOK},
                                    {'B', PROMOTE_BISHOP},
                                    {'N', PROMOTE_KNIGHT}};
    for (Move move : matches) {
      if (special(move) ==
          promo_type[toupper(move_string[move_string.length() - 1])]) {
        *output = move;
        return true;
      }
    }
    return false;

    // determine pawn promotion type
  }

  *output = matches[0];
  return true;
}

}  // namespace squares

}  // namespace exacto
