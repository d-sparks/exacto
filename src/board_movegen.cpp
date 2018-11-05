#include "board.h"

#include "bitboard.h"
#include "inlines.h"
#include "magics.h"
#include "masks.h"
#include "moves.h"
#include "squares.h"

namespace exacto {

// Generate legal moves given the current state of the board.
void Board::MoveGen(Move *move_list) { MoveGen(move_list, true); }

// Generate legal captures or check evasions given the current state of the
// board.
void Board::CapGen(Move *move_list) { MoveGen(move_list, false); }

// Main coordinating function for MoveGen.
void Board::MoveGen(Move *move_list, bool quiet_moves) {
  ind king_square = bitscan(pieces[wtm][KING]);
  Bitboard enemy_attacks = attackSetGen(!wtm);
  Bitboard pins = bishopPins(king_square) | rookPins(king_square);
  bool currently_in_check = pieces[wtm][KING] & enemy_attacks;

  if (currently_in_check) {
    evasionGen(&move_list, enemy_attacks, pins, king_square);
  } else {
    pawnGen(&move_list, pins, quiet_moves);
    knightGen(&move_list, pins, quiet_moves);
    bishopGen(&move_list, pins, quiet_moves);
    rookGen(&move_list, pins, quiet_moves);
    kingGen(&move_list, king_square, enemy_attacks, quiet_moves);
    if (pins) {
      pawnGenPinned(&move_list, pins, king_square, quiet_moves);
      bishopGenPinned(&move_list, pins, king_square, quiet_moves);
      rookGenPinned(&move_list, pins, king_square, quiet_moves);
    }
  }
  closeMoveList(&move_list);
}

// Returns true if the king is in check.
bool Board::in_check() { return (pieces[wtm][KING] & attackSetGen(!wtm)) != 0; }

// Generates the non-capture pawn moves
void Board::pawnGen(Move **move_list, Bitboard pins, bool quiet_moves) {
  pawnCaps(move_list, pins);
  if (quiet_moves) {
    Bitboard pawns = pieces[wtm][PAWN] & ~pins;
    if (wtm) {
      Bitboard b = (pawns << 8) & ~occupied;
      SerializePawn(move_list, b, REGULAR_MOVE, 8);
      SerializePawn(move_list, ((b & masks::RANK[2]) << 8) & ~occupied,
                    DOUBLE_PAWN_MOVE_W, 16);
    } else {
      Bitboard b = (pawns >> 8) & ~occupied;
      SerializePawn(move_list, b, REGULAR_MOVE, -8);
      SerializePawn(move_list, ((b & masks::RANK[5]) >> 8) & ~occupied,
                    DOUBLE_PAWN_MOVE_B, -16);
    }
  }
}

// Generate pawn capture moves
void Board::pawnCaps(Move **move_list, Bitboard pins) {
  Bitboard pawns = pieces[wtm][PAWN] & ~pins;
  if (wtm) {
    SerializePawn(move_list,
                  ((pawns & ~masks::FILE[7]) << 9) & pieces[BLACK][ALL],
                  REGULAR_MOVE, 9);
    SerializePawn(move_list,
                  ((pawns & ~masks::FILE[0]) << 7) & pieces[BLACK][ALL],
                  REGULAR_MOVE, 7);
  } else {
    SerializePawn(move_list,
                  ((pawns & ~masks::FILE[7]) >> 7) & pieces[WHITE][ALL],
                  REGULAR_MOVE, -7);
    SerializePawn(move_list,
                  ((pawns & ~masks::FILE[0]) >> 9) & pieces[WHITE][ALL],
                  REGULAR_MOVE, -9);
  }
  if (en_passant) {
    do {
      ind en_passant_square = bitscan(en_passant);
      Bitboard candidateSquares = wtm ? (en_passant >> 7 | en_passant >> 9)
                                : (en_passant << 7 | en_passant << 9);
      candidateSquares &= pawns & masks::KING_MOVES[en_passant_square];
      if (candidateSquares == 0) {
        break;
      }

      // We take care of pins like this now.  Presumably this code will execute
      // infrequently:

      //     +---+---+---+---+---+---+---+---+
      //     |   |   |   |   | : |   |   |   |
      //     +---+---+---+---+---+---+---+---+
      //     |   | r |   | P | p |   | K |   |
      //     +---+---+---+---+---+---+---+---+

      // If control has reached this point, we know either that there are two
      // pawns that are in the above configuration.

      Bitboard epRank = masks::RANK[wtm ? 4 : 3];
      Bitboard kingOnEPRank = pieces[wtm][KING] & epRank;
      Bitboard horizontalOnEPRank =
          (pieces[!wtm][ROOK] | pieces[!wtm][QUEEN]) & epRank;

      // At this point we know that our king, an enemy horizontal sliding piece,
      // and our two pawns are on this rank. The pawns must be adjacent, and
      // since we are assuming we are not in check, then we check for this
      // weird pin.

      if ((kingOnEPRank != 0) && (horizontalOnEPRank != 0) &&
          (popcount(occupied & epRank) >= 4)) {
        bool weirdPin = false;
        ind kingInd = bitscan(pieces[wtm][KING]);
        while (horizontalOnEPRank) {
          ind horizInd = bitscan(horizontalOnEPRank);
          Bitboard interceding = masks::INTERCEDING[horizInd][kingInd] & occupied;
          if (popcount(interceding) == 2 && (interceding & candidateSquares)) {
            weirdPin = true;
            break;
          }
          horizontalOnEPRank ^= exp_2(horizInd);
        }
        if (weirdPin) {
          break;
        }
      }

      // Finally, generate the moves.
      SerializeFromDest(move_list, candidateSquares, en_passant_square, PAWN,
                        EN_PASSANT_CAP);

    } while (false);
  }
}

// Generate legal moves for pawns that are pinned to the king.
void Board::pawnGenPinned(Move **move_list, Bitboard pins, ind king_square,
                           bool quiet_moves) {
  Bitboard pawns = pieces[wtm][PAWN] & pins;
  while (pawns) {
    ind square = bitscan(pawns);

    // Regular moves
    Bitboard pawnShift;
    if (wtm) {
      pawnShift = exp_2(square) << 8;
      if (quiet_moves && (squares::file(square) == squares::file(king_square))) {
        Bitboard moves = pawnShift & ~occupied;
        SerializePawn(move_list, moves, REGULAR_MOVE, 8);
        if (moves & masks::RANK[2]) {
          SerializePawn(move_list, (moves << 8) & ~occupied, DOUBLE_PAWN_MOVE_W,
                        16);
        }
      }
    } else {
      pawnShift = exp_2(square) >> 8;
      if (quiet_moves && (squares::file(square) == squares::file(king_square))) {
        Bitboard moves = pawnShift & ~occupied;
        SerializePawn(move_list, moves, REGULAR_MOVE, -8);
        if (moves & masks::RANK[5]) {
          SerializePawn(move_list, (moves >> 8) & ~occupied, DOUBLE_PAWN_MOVE_B,
                        -16);
        }
      }
    }

    // Captures
    Bitboard pawnThreats = ((pawnShift << 1) | (pawnShift >> 1)) &
                     masks::OPPOSITE[king_square][square];
    ind targetSquare = bitscan(pawnThreats);
    if (pawnThreats & pieces[!wtm][ALL]) {
      SerializeFromDest(move_list, exp_2(square), targetSquare,
                        board[targetSquare], REGULAR_MOVE);
    } else if (pawnThreats & en_passant) {
      SerializeFromDest(move_list, exp_2(square), targetSquare, PAWN,
                        EN_PASSANT_CAP);
    }

    pawns &= pawns - 1;
  }
}

// Generates all moves for knights. Accepts a boolean argument which specifies
// whether to return non-captures.
void Board::knightGen(Move **move_list, Bitboard pins, bool quiet_moves) {
  Bitboard knights = pieces[wtm][KNIGHT] & ~pins;
  while (knights) {
    ind i = bitscan(knights);
    Bitboard movesBB = masks::KNIGHT_MOVES[i] &
                 (quiet_moves ? ~pieces[wtm][ALL] : pieces[!wtm][ALL]);
    Serialize(move_list, movesBB, i);
    knights &= ~exp_2(i);
  }
}

// Generates all moves for bishops and the bishop moves for queens. Accepts a
// boolean argument which specifies whether to return non-captures.
void Board::bishopGen(Move **move_list, Bitboard pins, bool quiet_moves) {
  Bitboard sliders = (pieces[wtm][BISHOP] | pieces[wtm][QUEEN]) & ~pins;
  while (sliders) {
    ind i = bitscan(sliders);
    Bitboard movesBB = magics::bishopMoves(i, occupied);
    movesBB &= quiet_moves ? ~pieces[wtm][ALL] : pieces[!wtm][ALL];
    Serialize(move_list, movesBB, i);
    sliders &= ~exp_2(i);
  }
}

// Generates all moves for bishops and the bishop moves for queens that are
// valid despite an already recognized pin. Accepts a boolean argument which
// specifies whether to return non-captures.
void Board::bishopGenPinned(Move **move_list, Bitboard pins, ind king_square,
                             bool quiet_moves) {
  Bitboard sliders = (pieces[wtm][BISHOP] | pieces[wtm][QUEEN]) & pins;
  while (sliders) {
    ind i = bitscan(sliders);
    Bitboard validMoves =
        masks::OPPOSITE[king_square][i] | masks::INTERCEDING[king_square][i];
    validMoves &= magics::bishopMoves(i, occupied);
    validMoves &= quiet_moves ? ~pieces[wtm][ALL] : pieces[!wtm][ALL];
    Serialize(move_list, validMoves, i);
    sliders &= sliders - 1;
  }
}

// Generates all moves for rooks, and the rook moves for queens. Accepts a
// boolean argument which specifies whether to return non-captures.
void Board::rookGen(Move **move_list, Bitboard pins, bool quiet_moves) {
  Bitboard sliders = (pieces[wtm][ROOK] | pieces[wtm][QUEEN]) & ~pins;
  while (sliders) {
    ind i = bitscan(sliders);
    Bitboard movesBB = magics::rookMoves(i, occupied);
    movesBB &= quiet_moves ? ~pieces[wtm][ALL] : pieces[!wtm][ALL];
    Serialize(move_list, movesBB, i);
    sliders &= ~exp_2(i);
  }
}

// Generates all moves for rooks, and the rooks moves for queens that are valid
// despite an already recognized pin. Accepts a boolean argument which specifies
// whether to return non-captures.
void Board::rookGenPinned(Move **move_list, Bitboard pins, ind king_square,
                           bool quiet_moves) {
  Bitboard sliders = (pieces[wtm][ROOK] | pieces[wtm][QUEEN]) & pins;
  while (sliders) {
    ind i = bitscan(sliders);
    Bitboard validMoves =
        masks::OPPOSITE[king_square][i] | masks::INTERCEDING[king_square][i];
    validMoves &= magics::rookMoves(i, occupied);
    validMoves &= quiet_moves ? ~pieces[wtm][ALL] : pieces[!wtm][ALL];
    Serialize(move_list, validMoves, i);
    sliders &= sliders - 1;
  }
}

// Generates all the moves from a given position, including castling moves.
// Optionally, only generate captures by passing quiet_moves=false.
void Board::kingGen(Move **move_list, ind king_square, Bitboard enemy_attacks,
                     bool quiet_moves) {
  // Regular moves
  Bitboard valid_squares =
      ~enemy_attacks & (quiet_moves ? ~pieces[wtm][ALL] : pieces[!wtm][ALL]);
  Bitboard moves = masks::KING_MOVES[king_square] & valid_squares;
  Serialize(move_list, moves, king_square, KING_MOVE);
  // Castling
  if (quiet_moves && castling[wtm] != 0) {
    // We extend our reach to the right by one square from each valid move found
    // so far. If the square between the rook and destination square for a left
    // (queenside) castle is empty, we similarly extend our reach to the left.
    // From here we determine if the destination squares are safe, empty squares
    // which we are still have castling rights to.
    moves &= ~occupied;
    moves |= moves >> 1;
    if (~occupied & (exp_2(king_square) << 3)) {
      moves |= moves << 1;
    }
    moves &= castling[wtm] & valid_squares & ~occupied;
    Serialize(move_list, moves, king_square, CASTLE);
  }
}

// Returns a bitmap of all squares threatened by a given color on the current
// board. A piece is threatening any square it aims at or can move to, even
// while pinned.
Bitboard Board::attackSetGen(bool color) {
  Bitboard attack_set = 0;

  // Pawns
  if (color == WHITE) {
    attack_set |= (pieces[WHITE][PAWN] & ~masks::FILE[7]) << 9;
    attack_set |= (pieces[WHITE][PAWN] & ~masks::FILE[0]) << 7;
  } else {
    attack_set |= (pieces[BLACK][PAWN] & ~masks::FILE[7]) >> 7;
    attack_set |= (pieces[BLACK][PAWN] & ~masks::FILE[0]) >> 9;
  }
  // Knights
  Bitboard knights = pieces[color][KNIGHT];
  while (knights) {
    ind i = bitscan(knights);
    attack_set |= masks::KNIGHT_MOVES[i];
    knights &= knights - 1;
  }
  // Bishops and queen diagonal attacks
  Bitboard bishops = pieces[color][BISHOP] | pieces[color][QUEEN];
  while (bishops) {
    ind i = bitscan(bishops);
    attack_set |= magics::bishopMoves(i, occupied);
    bishops &= bishops - 1;
  }
  // Rooks and queen horizontal/vertical moves
  Bitboard rooks = pieces[color][ROOK] | pieces[color][QUEEN];
  while (rooks) {
    ind i = bitscan(rooks);
    attack_set |= magics::rookMoves(i, occupied);
    rooks &= rooks - 1;
  }
  // King moves
  attack_set |= masks::KING_MOVES[bitscan(pieces[color][KING])];

  return attack_set;
} 

// Generates the moves to a given square. King moves will only generate moves to
// the given square if they are a capture.
void Board::generateMovesTo(Move **move_list,
                            ind dest, ind defender,
                            Bitboard pins,
                            Bitboard enemy_attacks) {
  // Pawn moves
  Bitboard dest_bb = exp_2(dest);
  Bitboard potential_pawns = wtm ? (dest_bb >> 8) : (dest_bb << 8);
  Bitboard pawns = pieces[wtm][PAWN] & ~pins;

  if (defender) {
    // Pawn captures
    if (pawns & ((potential_pawns & ~masks::FILE[7]) << 1)) {
      SerializePawn(move_list, dest_bb, REGULAR_MOVE, wtm ? 7 : -9);
    }
    if (pawns & ((potential_pawns & ~masks::FILE[0]) >> 1)) {
      SerializePawn(move_list, dest_bb, REGULAR_MOVE, wtm ? 9 : -7);
    }
  } else if (dest == bitscan(en_passant)) {
    // Pawn en passant captures
    SerializeFromDest(move_list,
                      pawns & ((potential_pawns & ~masks::FILE[0]) << 1), dest,
                      PAWN, EN_PASSANT_CAP);
    SerializeFromDest(move_list,
                      pawns & ((potential_pawns & ~masks::FILE[7]) >> 1), dest,
                      PAWN, EN_PASSANT_CAP);
  } else {
    if (pawns & potential_pawns) {
      // Pawn regular moves
      SerializePawn(move_list, dest_bb, REGULAR_MOVE, wtm ? 8 : -8);
    } else if (potential_pawns & ~occupied) {
      // Pawn double moves
      if (wtm && squares::rank(dest) == 3) {
        SerializeFromDest(move_list, pawns & (potential_pawns >> 8), dest, NONE,
                          DOUBLE_PAWN_MOVE_W);
      } else if (!wtm && squares::rank(dest) == 4) {
        SerializeFromDest(move_list, pawns & (potential_pawns << 8), dest, NONE,
                          DOUBLE_PAWN_MOVE_B);
      }
    }
  }

  // Knights, bishops, and rooks are very simple.
  Bitboard knights = masks::KNIGHT_MOVES[dest] & pieces[wtm][KNIGHT] & ~pins;
  SerializeFromDest(move_list, knights, dest, defender, REGULAR_MOVE);
  Bitboard bishops = magics::bishopMoves(dest, occupied) &
               (pieces[wtm][BISHOP] | pieces[wtm][QUEEN]) & ~pins;
  SerializeFromDest(move_list, bishops, dest, defender, REGULAR_MOVE);
  Bitboard rooks = magics::rookMoves(dest, occupied) &
             (pieces[wtm][ROOK] | pieces[wtm][QUEEN]) & ~pins;
  SerializeFromDest(move_list, rooks, dest, defender, REGULAR_MOVE);

  // Pinned pieces separately
  if (pins) {
    // Pawns are annoying
    // Rooks and bishops less so
    // TODO!
  }
}

// Assumes the king is in check and generates all legal ways to evade the check:
// move the king, kill the attacker or intercept.
void Board::evasionGen(Move **move_list, Bitboard enemy_attacks, Bitboard pins,
                        ind king_square) {
  Bitboard attackers[QUEEN] = {0};

  // Find all pieces attacking the king
  if (wtm) {
    attackers[PAWN] = pieces[BLACK][PAWN] &
                      (pieces[WHITE][KING] << 7 | pieces[WHITE][KING] << 9);
  } else {
    attackers[PAWN] = pieces[WHITE][PAWN] &
                      (pieces[BLACK][KING] >> 7 | pieces[BLACK][KING] >> 9);
  }
  attackers[PAWN] &= masks::KING_MOVES[king_square];
  attackers[KNIGHT] = masks::KNIGHT_MOVES[king_square] & pieces[!wtm][KNIGHT];
  attackers[BISHOP] = magics::bishopMoves(king_square, occupied) &
                      (pieces[!wtm][BISHOP] | pieces[!wtm][QUEEN]);
  attackers[ROOK] = magics::rookMoves(king_square, occupied) &
                    (pieces[!wtm][ROOK] | pieces[!wtm][QUEEN]);
  attackers[ALL] =
      attackers[PAWN] | attackers[KNIGHT] | attackers[BISHOP] | attackers[ROOK];

  // If only one piece is attacking the king, can intercept or capture
  if (popcount(attackers[ALL]) == 1) {
    Bitboard intercept = attackers[ALL];
    if (attackers[BISHOP] | attackers[ROOK]) {
      intercept |= masks::INTERCEDING[king_square][bitscan(attackers[ALL])];
    } else if (attackers[PAWN] && en_passant) {
      ind en_passant_square = bitscan(en_passant);
      ind attacker_square = bitscan(attackers[PAWN]);
      Bitboard candidate_pawns = masks::KING_MOVES[en_passant_square] &
          masks::KING_MOVES[attacker_square];
      candidate_pawns &= pieces[wtm][PAWN] & ~pins;
      if (candidate_pawns) {
        SerializeFromDest(move_list, candidate_pawns, en_passant_square, PAWN,
                          EN_PASSANT_CAP);
      }
    }

    // Generate interception moves
    while (intercept) {
      ind dest = bitscan(intercept);
      generateMovesTo(move_list, dest, board[dest], pins, enemy_attacks);
      intercept &= intercept - 1;
    }
  }

  // In any case, the king can move to safe squares. The for loop here covers
  // the case of the a sliding piece aimed at a king - it is not safe to move to
  // the opposite square in that case.
  Bitboard king_moves =
      masks::KING_MOVES[king_square] & ~pieces[wtm][ALL] & ~enemy_attacks;
  for (Bitboard x = king_moves; x; x &= x - 1) {
    ind i = bitscan(x);
    if (masks::OPPOSITE[i][king_square] &
        (attackers[BISHOP] | attackers[ROOK])) {
      king_moves &= x - 1;
    }
  }
  Serialize(move_list, king_moves, king_square, KING_MOVE);
}

// Gets the pieces that are currently pinned to the king by a diagonally moving
// piece.
Bitboard Board::bishopPins(ind king_square) {
  Bitboard pins = 0;
  Bitboard candidatePins =
      magics::bishopMoves(king_square, occupied) & pieces[wtm][ALL];
  while (candidatePins) {
    ind i = bitscan(candidatePins);
    Bitboard candidateMoves = magics::bishopMoves(i, occupied);
    Bitboard king_xray_attacks =
        magics::bishopMoves(king_square, occupied & ~exp_2(i));
    if (candidateMoves & king_xray_attacks &
        (pieces[!wtm][BISHOP] | pieces[!wtm][QUEEN])) {
      pins |= exp_2(i);
    }
    candidatePins &= candidatePins - 1;
  }
  return pins;
}

// Gets the pieces that are currently pinned to the king by a horizontally
// moving piece.
Bitboard Board::rookPins(ind king_square) {
  Bitboard pins = 0;
  Bitboard candidatePins =
      magics::rookMoves(king_square, occupied) & pieces[wtm][ALL];
  while (candidatePins) {
    ind i = bitscan(candidatePins);
    Bitboard candidateMoves = magics::rookMoves(i, occupied);
    candidatePins &= candidatePins - 1;
    Bitboard king_xray_attacks =
        magics::rookMoves(king_square, occupied & ~exp_2(i));
    if (candidateMoves & king_xray_attacks &
        (pieces[!wtm][ROOK] | pieces[!wtm][QUEEN])) {
      pins |= exp_2(i);
    }
  }
  return pins;
}

// Serialize turns a Bitboard into moves.
void Board::Serialize(Move **move_list, Bitboard b, ind source) {
  Serialize(move_list, b, source, NONE);
}

void Board::Serialize(Move **move_list, Bitboard b, ind source, ind special) {
  while (b) {
    ind dest = bitscan(b);
    *((*move_list)++) = moves::make(source, dest, board[source], board[dest],
                                    NONE, NONE, special);
    b &= b - 1;
  }
}

// SerializeFromDest turns a Bitboard into moves, where the given Bitboard are
// source squares. This can be used to generate intercepting moves to checks or
// for en_passant pawn captures, where one knows the destination in advance and
// generates moves to it.
void Board::SerializeFromDest(Move **move_list,
                              Bitboard b,
                              ind dest,
                              ind defender,
                              ind special) {
  while (b) {
    ind source = bitscan(b);
    *((*move_list)++) =
        moves::make(source, dest, board[source], defender, NONE, NONE, special);
    b &= b - 1;
  }
}

// SerializePawn Serializes a move Bitboard based on a "delta" as a list of
// moves. For each set bit with index i, creates moves from i-delta to i.
// Handles pawn promotion moves, but expects double pawn moves and en-passant
// capture special flags to be passed in. Note that promotions and these types
// of moves are disjoint.
void Board::SerializePawn(Move **move_list,
                          Bitboard b,
                          ind special,
                          int delta) {
  // non-promotion moves
  Bitboard pawns = b & ~masks::promo_rank(wtm);
  while (pawns) {
    ind i = bitscan(pawns);
    *((*move_list)++) =
        moves::make((int)i - delta, i, PAWN, board[i], 0, 0, special);
    pawns ^= exp_2(i);
  }
  // promotion moves
  pawns = b & masks::promo_rank(wtm);
  while (pawns) {
    ind i = bitscan(pawns);
    for (ind special = PROMOTE_KNIGHT; special <= PROMOTE_QUEEN; special++) {
      *((*move_list)++) =
          moves::make((int)i - delta, i, PAWN, board[i], 0, 0, special);
    }
    pawns ^= exp_2(i);
  }
}

// The end of a move_list array is signified by a 0 move. This helper adds that
// 0 move to the list.
void Board::closeMoveList(Move **move_list) { *((*move_list)++) = 0; }

}  // namespace exacto
