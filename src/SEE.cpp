#include "SEE.h"

#include "bitboard.h"
#include "board.h"
#include "defines.h"
#include "inlines.h"
#include "magics.h"
#include "masks.h"
#include "moves.h"
#include "squares.h"

namespace exacto {

namespace SEE {

int16_t piece_values[7] = {
    0, PAWN_VAL, KNIGHT_VAL, BISHOP_VAL, ROOK_VAL, QUEEN_VAL, MATESCORE,
};

int16_t see(Board* game, Move move) {
  Bitboard source = exp_2(moves::source(move));
  ind dest = moves::dest(move);
  ind attacker = moves::attacker(move) % 8;
  int16_t score = piece_values[moves::defender(move) % 8];

  MakeMove(game, source, attacker);
  score -= next(game, piece_values[attacker], dest);
  UnmakeMove(game, source, attacker);

  return score;
}

int16_t next(Board* board, int16_t previous_val, ind square) {
  ind attacker_square = least_valuable_attacker_square(board, square);
  if (attacker_square > 63) {
    return 0;
  }
  ind attacker = board->board[attacker_square] % 8;
  Bitboard source = exp_2(attacker_square);
  MakeMove(board, source, attacker);
  int16_t value =
      std::max(0, previous_val - next(board, piece_values[attacker], square));
  UnmakeMove(board, source, attacker);
  return value;
}

void MakeMove(Board* board, Bitboard source, ind attacker) {
  board->pieces[board->wtm][attacker] &= ~source;
  board->occupied &= ~source;
  board->wtm = !board->wtm;
}

void UnmakeMove(Board* board, Bitboard source, ind attacker) {
  board->wtm = !board->wtm;
  board->pieces[board->wtm][attacker] |= source;
  board->occupied |= source;
}

ind least_valuable_attacker_square(Board* board, ind square) {
  Bitboard candidates = 0;
  do {
    bool wtm = board->wtm;
    candidates = masks::PAWN_CHECKS[!wtm][square] & board->pieces[wtm][PAWN];
    if (candidates != 0) break;
    candidates = masks::KNIGHT_MOVES[square] & board->pieces[wtm][KNIGHT];
    if (candidates != 0) break;
    Bitboard bishop_moves = magics::BishopMoves(square, board->occupied);
    candidates = bishop_moves & board->pieces[wtm][BISHOP];
    if (candidates != 0) break;
    Bitboard rook_moves = magics::RookMoves(square, board->occupied);
    candidates = rook_moves & board->pieces[wtm][ROOK];
    if (candidates != 0) break;
    candidates = (bishop_moves | rook_moves) & board->pieces[wtm][QUEEN];
    if (candidates != 0) break;
    candidates = masks::KING_MOVES[square] & board->pieces[wtm][KING];
    if (candidates != 0) break;
    return 255;
  } while (false);
  return bitscan(candidates);
}

}  // namespace SEE

}  // namespace exacto
