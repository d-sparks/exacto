#pragma once
#include "cboard.h"
#include "squares.h"
#include "bb.h"
#include "masks.cpp"
#include "moves.cpp"
#include "magics.cpp"

using namespace std;

// Generate legal moves given the current state of the board.
void CBoard::moveGen(mv * moveList, bool quietMoves) {
    ind kingSquare = bitscan(pieces[wtm][KING]);
    BB enemyAttacks = attackSetGen(!wtm);
    BB pins = bishopPins(kingSquare) | rookPins(kingSquare);
    bool inCheck = pieces[wtm][KING] & enemyAttacks;

    if(inCheck) {
        evasionGen(&moveList, enemyAttacks, pins, kingSquare);
    } else {
        pawnGen(&moveList, pins, quietMoves);
        knightGen(&moveList, pins, quietMoves);
        bishopGen(&moveList, pins, quietMoves);
        rookGen(&moveList, pins, quietMoves);
        kingGen(&moveList, kingSquare, enemyAttacks, quietMoves);
        if(pins) {
            pawnGenPinned(&moveList, pins, kingSquare, quietMoves);
            bishopGenPinned(&moveList, pins, kingSquare, quietMoves);
            rookGenPinned(&moveList, pins, kingSquare, quietMoves);
        }
    }
    closeMoveList(&moveList);
}

// Generates the non-capture pawn moves
void CBoard::pawnGen(mv **moveList, BB pins, bool quietMoves) {
    pawnCaps(moveList, pins);
    if(quietMoves) {
        if(wtm) {
            BB b = (pieces[WHITE][PAWN] << 8) & ~occupied;
            serializePawn(moveList, b, REGULAR_MOVE, 8);
            serializePawn(moveList, ((b & masks::RANK[2]) << 8) & ~occupied, DOUBLE_PAWN_MOVE_W, 16);
        } else {
            BB b = (pieces[BLACK][PAWN] >> 8) & ~occupied;
            serializePawn(moveList, b, REGULAR_MOVE, -8);
            serializePawn(moveList, ((b & masks::RANK[5]) >> 8) & ~occupied, DOUBLE_PAWN_MOVE_B, -16);
        }
    }
}

// Generate pawn capture moves
void CBoard::pawnCaps(mv **moveList, BB pins) {
    BB pawns = pieces[wtm][PAWN] & ~pins;
    if(wtm) {
        serializePawn(moveList, ((pawns & ~masks::FILE[0]) << 9) & pieces[BLACK][ALL], REGULAR_MOVE, 9);
        serializePawn(moveList, ((pawns & ~masks::FILE[7]) << 7) & pieces[BLACK][ALL], REGULAR_MOVE, 7);
    } else {
        serializePawn(moveList, ((pawns & ~masks::FILE[0]) >> 7) & pieces[WHITE][ALL], REGULAR_MOVE, -7);
        serializePawn(moveList, ((pawns & ~masks::FILE[7]) >> 9) & pieces[WHITE][ALL], REGULAR_MOVE, -9);
    }
    if(enPassant) {
        do {
            ind enPassantSquare = bitscan(enPassant);
            BB candidateSquares = wtm ? (enPassant >> 7 | enPassant >> 9) : (enPassant << 7 | enPassant << 9);
            candidateSquares &= pawns & masks::KING_MOVES[enPassantSquare];
            if(candidateSquares == 0) {
                break;
            }

            // We take care of pins like this now.  Presumably this code will execute infrequently:

            //     +---+---+---+---+---+---+---+---+
            //     |   |   |   |   | : |   |   |   |
            //     +---+---+---+---+---+---+---+---+
            //     |   | r |   | P | p |   | K |   |
            //     +---+---+---+---+---+---+---+---+

            // If control has reached this point, we know either that there are two pawns that are
            // in the above configuration.

            BB epRank = masks::RANK[wtm ? 5 : 2];
            bool kingOnEPRank = (pieces[wtm][KING] & epRank) != 0;
            bool horizontalOnEPRank = ((pieces[!wtm][ROOK] | pieces[!wtm][ROOK]) & epRank) != 0;

            // At this point we know that our king, an enemy horizontal sliding piece, and our two pawns
            // are on this rank. The pawns must be adjacent, and since we are assuming we are not in
            // check, then we deduce: if there are exactly four pieces on this rank, we are in the above
            // situation.

            if(kingOnEPRank && horizontalOnEPRank && popcount(occupied) == 4) {
                break;
            }

            // Finally, generate the moves.
            serializeFromDest(moveList, candidateSquares, enPassantSquare, PAWN, EN_PASSANT_CAP);

        } while(false);
    }
}

// Generate legal moves for pawns that are pinned to the king.
void CBoard::pawnGenPinned(mv **moveList, BB pins, ind kingSquare, bool quietMoves) {
    BB pawns = pieces[wtm][PAWN] & pins;
    while(pawns) {
        ind square = bitscan(pawns);

        // Regular moves
        BB pawnShift;
        if(wtm) {
            pawnShift = exp_2(square) << 8;
            if(quietMoves && squares::file(square) == squares::file(kingSquare)) {
                BB moves = pawnShift & ~occupied;
                serializePawn(moveList, moves, REGULAR_MOVE, 8);
                if(moves & masks::RANK[2]) {
                    serializePawn(moveList, (moves << 8) & ~occupied, DOUBLE_PAWN_MOVE_W, 16);
                }
            }
        } else {
            pawnShift = exp_2(square) >> 8;
            if(quietMoves && squares::file(square) == squares::file(kingSquare)) {
                BB moves = pawnShift & ~occupied;
                serializePawn(moveList, moves, REGULAR_MOVE, -8);
                if(moves & masks::RANK[5]) {
                    serializePawn(moveList, (moves >> 8) & ~occupied, DOUBLE_PAWN_MOVE_B, -16);
                }
            }
        }

        // Captures
        BB pawnThreats = ((pawnShift << 1) | (pawnShift >> 1)) & masks::OPPOSITE[kingSquare][square];
        ind targetSquare = bitscan(pawnThreats);
        if(pawnThreats & pieces[WHITE][ALL]) {
            serializeFromDest(moveList, exp_2(square), targetSquare, board[targetSquare], REGULAR_MOVE);
        } else if(pawnThreats & enPassant) {
            serializeFromDest(moveList, exp_2(square), targetSquare, PAWN, EN_PASSANT_CAP);
        }

        pawns &= pawns - 1;
    }
}

// Generates all moves for knights. Accepts a boolean argument which specifies whether to return
// non-captures.
void CBoard::knightGen(mv **moveList, BB pins, bool quietMoves) {
    BB knights = pieces[wtm][KNIGHT] & ~pins;
    while(knights) {
        ind i = bitscan(knights);
        BB movesBB = masks::KNIGHT_MOVES[i] & (quietMoves? ~pieces[wtm][ALL] : pieces[!wtm][ALL]);
        serialize(moveList, movesBB, i);
        knights &= ~exp_2(i);
    }
}

// Generates all moves for bishops and the bishop moves for queens. Accepts a boolean argument which
// specifies whether to return non-captures.
void CBoard::bishopGen(mv **moveList, BB pins, bool quietMoves) {
    BB sliders = (pieces[wtm][BISHOP] | pieces[wtm][QUEEN]) & ~pins;
    while(sliders) {
        ind i = bitscan(sliders);
        BB movesBB = magics::bishopMoves(i, occupied);
        movesBB &= quietMoves ? ~pieces[wtm][ALL] : pieces[!wtm][ALL];
        serialize(moveList, movesBB, i);
        sliders &= ~exp_2(i);
    }
}

// Generates all moves for bishops and the bishop moves for queens that are valid despite an already
// recognized pin. Accepts a boolean argument which specifies whether to return non-captures.
void CBoard::bishopGenPinned(mv **moveList, BB pins, ind kingSquare, bool quietMoves) {
    BB sliders = (pieces[wtm][BISHOP] | pieces[wtm][QUEEN]) & pins;
    while(sliders) {
        ind i = bitscan(sliders);
        BB validMoves = masks::OPPOSITE[kingSquare][i] | masks::INTERCEDING[kingSquare][i];
        validMoves &= magics::bishopMoves(i, occupied);
        validMoves &= quietMoves ? ~pieces[wtm][ALL] : pieces[!wtm][ALL];
        serialize(moveList, validMoves, i);
        sliders &= sliders - 1;
    }
}

// Generates all moves for rooks, and the rook moves for queens. Accepts a boolean argument which
// specifies whether to return non-captures.
void CBoard::rookGen(mv **moveList, BB pins, bool quietMoves) {
    BB sliders = (pieces[wtm][ROOK] | pieces[wtm][QUEEN]) & ~pins;
    while(sliders) {
        ind i = bitscan(sliders);
        BB movesBB = magics::rookMoves(i, occupied);
        movesBB &= quietMoves ? ~pieces[wtm][ALL] : pieces[!wtm][ALL];
        serialize(moveList, movesBB, i);
        sliders &= ~exp_2(i);
    }
}

// Generates all moves for rooks, and the rooks moves for queens that are valid despite an already
// recognized pin. Accepts a boolean argument which specifies whether to return non-captures.
void CBoard::rookGenPinned(mv **moveList, BB pins, ind kingSquare, bool quietMoves) {
    BB sliders = (pieces[wtm][ROOK] | pieces[wtm][QUEEN]) & pins;
    while(sliders) {
        ind i = bitscan(sliders);
        BB validMoves = masks::OPPOSITE[kingSquare][i] | masks::INTERCEDING[kingSquare][i];
        validMoves &= magics::rookMoves(i, occupied);
        validMoves &= quietMoves ? ~pieces[wtm][ALL] : pieces[!wtm][ALL];
        serialize(moveList, validMoves, i);
        sliders &= sliders - 1;
    }
}

// Generates all the moves from a given position, including castling moves. Optionally, only
// generate captures by passing quietMoves=false.
void CBoard::kingGen(mv **moveList, ind kingSquare, BB enemyAttacks, bool quietMoves) {
    // Regular moves
    BB validSquares = ~enemyAttacks & (quietMoves ? ~pieces[wtm][ALL] : pieces[!wtm][ALL]);
    BB moves = masks::KING_MOVES[kingSquare] & validSquares;
    serialize(moveList, moves, kingSquare);
    // Castling
    if(quietMoves && castling[wtm] != 0) {
        // We extend our reach to the right by one square from each valid move found so far. If the
        // square between the rook and destination square for a left (queenside) castle is empty, we
        // similarly extend our reach to the left. From here we determine if the destination squares
        // are safe, empty squares which we are still have castling rights to.
        moves &= ~occupied;
        moves |= moves >> 1;
        if(~occupied & (exp_2(kingSquare) << 3)) {
            moves |= moves << 1;
        }
        moves = (moves & castling[wtm]) & validSquares;
        // TODO: add special flag by overloading serialize
        serialize(moveList, moves, kingSquare, CASTLE);
    }
}

// Returns a bitmap of all squares threatened by a given color on the current board. A piece is
// threatening any square it aims at or can move to, even while pinned.
BB CBoard::attackSetGen(bool color) {
    BB attackSet = 0;

    // Pawns
    if(color == WHITE) {
        attackSet |= (pieces[WHITE][PAWN] & ~masks::FILE[0]) << 9;
        attackSet |= (pieces[WHITE][PAWN] & ~masks::FILE[7]) << 7;
    } else {
        attackSet |= (pieces[BLACK][PAWN] & ~masks::FILE[0]) >> 7;
        attackSet |= (pieces[BLACK][PAWN] & ~masks::FILE[7]) >> 9;
    }
    // Knights
    BB knights = pieces[color][KNIGHT];
    while(knights) {
        ind i = bitscan(knights);
        attackSet |= masks::KNIGHT_MOVES[i];
        knights &= knights - 1;
    }
    // Bishops and queen diagonal attacks
    BB bishops = pieces[color][BISHOP] | pieces[color][QUEEN];
    while(bishops) {
        ind i = bitscan(bishops);
        attackSet |= magics::bishopMoves(i, occupied);
        bishops &= bishops - 1;
    }
    // Rooks and queen horizontal/vertical moves
    BB rooks = pieces[color][ROOK] | pieces[color][QUEEN];
    while(rooks) {
        ind i = bitscan(rooks);
        attackSet |= magics::rookMoves(i, occupied);
        rooks &= rooks - 1;
    }
    // King moves
    attackSet |= masks::KING_MOVES[bitscan(pieces[color][KING])];

    return attackSet;
}

// Generates the moves to a given square. King moves will only generate moves to the given square if
// they are a capture.
void CBoard::generateMovesTo(mv **moveList, ind dest, ind defender, BB pins, BB enemyAttacks) {

    // Pawn moves
    BB destBB = exp_2(dest);
    BB potentialPawns = wtm ? destBB >> 8 : destBB << 8;
    BB pawns = pieces[wtm][PAWN];
    if(defender) {
        // Pawn captures
        if(pawns & ((potentialPawns & ~masks::FILE[7]) << 1)) {
            serializePawn(moveList, destBB, REGULAR_MOVE, wtm ? 7 : -9);
        }
        if(pawns & ((potentialPawns & ~masks::FILE[0]) >> 1)) {
            serializePawn(moveList, destBB, REGULAR_MOVE, wtm ? 9 : -7);
        }
    } else if(dest == bitscan(enPassant)) {
        // Pawn en passant captures
        serializeFromDest(moveList, pawns & ((potentialPawns & ~masks::FILE[7]) << 1), dest, PAWN, EN_PASSANT_CAP);
        serializeFromDest(moveList, pawns & ((potentialPawns & ~masks::FILE[0]) >> 1), dest, PAWN, EN_PASSANT_CAP);
    } else {
        if(pawns & potentialPawns) {
            // Pawn regular moves
            serializePawn(moveList, destBB, REGULAR_MOVE, wtm ? 8 : -8);
        } else if(potentialPawns & ~occupied) {
            // Pawn double moves
            if(wtm && squares::file(dest) == 3) {
                serializeFromDest(moveList, pawns & (potentialPawns >> 8), dest, NONE, DOUBLE_PAWN_MOVE_W);
            } else if(!wtm && squares::file(dest) == 4) {
                serializeFromDest(moveList, pawns & (potentialPawns << 8), dest, NONE, DOUBLE_PAWN_MOVE_B);
            }
        }
    }

    // Knights, bishops, and rooks are very simple.
    BB knights = masks::KNIGHT_MOVES[dest] & pieces[wtm][KNIGHT];
    serializeFromDest(moveList, knights, dest, defender, REGULAR_MOVE);
    BB bishops = magics::bishopMoves(dest, occupied) & (pieces[wtm][BISHOP] | pieces[wtm][QUEEN]);
    serializeFromDest(moveList, bishops, dest, defender, REGULAR_MOVE);
    BB rooks = magics::rookMoves(dest, occupied) & (pieces[wtm][ROOK] | pieces[wtm][QUEEN]);
    serializeFromDest(moveList, rooks, dest, defender, REGULAR_MOVE);

}

// Assumes the king is in check and generates all legal ways to evade the check: move the king, kill
// the attacker or intercept.
void CBoard::evasionGen(mv **moveList, BB enemyAttacks, BB pins, ind kingSquare) {
    BB attackers[QUEEN] = { 0 };

    // Find all pieces attacking the king
    if(wtm) {
        attackers[PAWN] = pieces[BLACK][PAWN] & (pieces[WHITE][KING] >> 7 | pieces[WHITE][KING] >> 9);
    } else {
        attackers[PAWN] = pieces[WHITE][PAWN] & (pieces[BLACK][KING] << 7 | pieces[BLACK][KING] << 9);
    }
    attackers[PAWN] &= masks::KING_MOVES[kingSquare];
    attackers[KNIGHT] = masks::KNIGHT_MOVES[kingSquare] & pieces[!wtm][KNIGHT];
    attackers[BISHOP] = magics::bishopMoves(kingSquare, occupied) & (pieces[!wtm][BISHOP] | pieces[!wtm][QUEEN]);
    attackers[ROOK] = magics::rookMoves(kingSquare, occupied) & (pieces[!wtm][ROOK] | pieces[!wtm][QUEEN]);
    attackers[ALL] = attackers[PAWN] | attackers[KNIGHT] | attackers[BISHOP] | attackers[ROOK];

    // If only one piece is attacking the king, can intercept or capture
    if(popcount(attackers[ALL]) == 1) {
        BB intercept = 0;
        if(attackers[BISHOP] | attackers[ROOK]) {
            intercept = masks::INTERCEDING[kingSquare][bitscan(attackers[ALL])];
        }

        // Generate interception moves
        while(intercept) {
            ind dest = bitscan(intercept);
            generateMovesTo(moveList, dest, board[dest], pins, enemyAttacks);
            intercept &= intercept - 1;
        }
    }

    // In any case, the king can move to safe squares
    BB kingMoves = masks::KING_MOVES[kingSquare] & ~pieces[wtm][ALL] & ~enemyAttacks;
    serialize(moveList, kingMoves, kingSquare);
}

// Gets the pieces that are currently pinned to the king by a diagonally moving piece.
BB CBoard::bishopPins(ind kingSquare) {
    BB pins = 0;
    BB candidatePins = magics::bishopMoves(kingSquare, occupied) & pieces[wtm][ALL];
    while(candidatePins) {
        ind i = bitscan(candidatePins);
        BB candidateMoves = magics::bishopMoves(i, occupied);
        candidatePins &= candidatePins - 1;
        BB kingXRayAttacks = magics::bishopMoves(kingSquare, occupied & ~exp_2(i));
        if(candidateMoves & kingXRayAttacks & (pieces[!wtm][BISHOP] | pieces[!wtm][QUEEN])) {
            pins |= exp_2(i);
        }
    }
    return pins;
}

// Gets the pieces that are currently pinned to the king by a horizontally moving piece.
BB CBoard::rookPins(ind kingSquare) {
    BB pins = 0;
    BB candidatePins = magics::rookMoves(kingSquare, occupied) & pieces[wtm][ALL];
    while(candidatePins) {
        ind i = bitscan(candidatePins);
        BB candidateMoves = magics::rookMoves(i, occupied);
        candidatePins &= candidatePins - 1;
        BB kingXRayAttacks = magics::rookMoves(kingSquare, occupied & ~exp_2(i));
        if(candidateMoves & kingXRayAttacks & (pieces[!wtm][ROOK] | pieces[!wtm][QUEEN])) {
            pins |= exp_2(i);
        }
    }
    return pins;
}

// serialize turns a bitboard into moves.
void CBoard::serialize(mv **moveList, BB b, ind source) {
    serialize(moveList, b, source, NONE);
}

void CBoard::serialize(mv **moveList, BB b, ind source, ind special) {
    while(b) {
        ind dest = bitscan(b);
        *((*moveList)++) = moves::make(source, dest, board[source], board[dest], NONE, NONE, special);
        b &= b - 1;
    }
}

// serializeFromDest turns a bitboard into moves, where the given bitboard are source squares. This
// can be used to generate intercepting moves to checks or for enPassant pawn captures, where one
// knows the destination in advance and generates moves to it.
void CBoard::serializeFromDest(mv **moveList, BB b, ind dest, ind defender, ind special) {
    while(b) {
        ind source = bitscan(b);
        *((*moveList)++) = moves::make(source, dest, board[source], defender, NONE, NONE, special);
        b &= b - 1;
    }
}

// serializePawn serializes a move bitboard based on a "delta" as a list of moves. For each set bit
// with index i, creates moves from i-delta to i. Handles pawn promotion moves, but expects double
// pawn moves and en-passant capture special flags to be passed in. Note that promotions and these
// types of moves are disjoint.
void CBoard::serializePawn(mv **moveList, BB b, ind special, int delta) {
    // non-promotion moves
    BB pawns = b & ~masks::promoRank(wtm);
    while(pawns) {
        ind i = bitscan(pawns);
        *((*moveList)++) = moves::make((int)i - delta, i, PAWN, board[i], 0, 0, special);
        pawns ^= exp_2(i);
    }
    // promotion moves
    pawns = b & masks::promoRank(wtm);
    while(pawns) {
        ind i = bitscan(pawns);
        for(ind special = PROMOTE_QUEEN; special <= PROMOTE_KNIGHT; special++) {
            *((*moveList)++) = moves::make((int)i - delta, i, PAWN, board[i], 0, 0, special);
        }
        pawns ^= exp_2(i);
    }
}

// The end of a moveList array is signified by a 0 move. This helper adds that 0 move to the list.
void CBoard::closeMoveList(mv **moveList) {
    *((*moveList)++) = 0;
}
