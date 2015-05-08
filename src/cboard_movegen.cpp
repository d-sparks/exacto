#pragma once
#include "cboard.h"
#include "squares.h"
#include "bb.h"
#include "masks.cpp"
#include "moves.cpp"
#include "magics.cpp"

using namespace std;

// Generate legal moves given the current state of the board.
void CBoard::moveGen(mv * moveList) {

    // Determine pinned pieces
    ind kingSquare = bitscan(pieces[wtm][KING]);
    BB pinnedByBishop = bishopPins(kingSquare);
    BB pinnedByRook = rookPins(kingSquare);
    BB pins = pinnedByRook | pinnedByBishop;

    // in check? else:
    pawnGen(&moveList, pins);
    pawnCaps(&moveList, pins);

    knightGen(&moveList, pins, true);

}

// Generates the non-capture pawn moves
void CBoard::pawnGen(mv **moveList, BB pins) {
    if(wtm) {
        BB b = (pieces[wtm][PAWN] << 8) & ~occupied;
        serializePawn(moveList, b, REGULAR_MOVE, 8);
        serializePawn(moveList, ((b & masks::RANK[2]) << 8) & ~occupied, DOUBLE_PAWN_MOVE_W, 16);
    } else {
        BB b = (pieces[wtm][PAWN] >> 8) & ~occupied;
        serializePawn(moveList, b, REGULAR_MOVE, -8);
        serializePawn(moveList, ((b & masks::RANK[5]) >> 8) & ~occupied, DOUBLE_PAWN_MOVE_W, -16);
    }
}

// Generate pawn capture moves
void CBoard::pawnCaps(mv **moveList, BB pins) {
    if(wtm) {
        BB pawns = pieces[wtm][PAWN];
        serializePawn(moveList, ((pawns & ~masks::FILE[0]) << 7) & pieces[BLACK][ALL], REGULAR_MOVE, 7);
        serializePawn(moveList, ((pawns & ~masks::FILE[7]) << 9) & pieces[BLACK][ALL], REGULAR_MOVE, 9);
    } else {
        BB pawns = pieces[wtm][PAWN];
        serializePawn(moveList, ((pawns & ~masks::FILE[0]) >> 9) & pieces[WHITE][ALL], REGULAR_MOVE, 9);
        serializePawn(moveList, ((pawns & ~masks::FILE[7]) >> 7) & pieces[WHITE][ALL], REGULAR_MOVE, 7);
    }
    // en-passant captures not yet supported
}

// Generates all moves for knights. Accepts a boolean argument which specifies whether to return
// non-captures.
void CBoard::knightGen(mv **moveList, BB pins, bool quietMoves) {
    BB knights = pieces[wtm][KNIGHT];
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
    BB sliders = pieces[wtm][BISHOP] | pieces[wtm][QUEEN];
    while(sliders) {
        ind i = bitscan(sliders);
        BB movesBB = magics::bishopMoves(i, occupied);
        movesBB &= quietMoves ? ~pieces[wtm][ALL] : pieces[!wtm][ALL];
        serialize(moveList, movesBB, i);
        sliders &= ~exp_2(i);
    }
}

// Generates all moves for rooks, and the rook moves for queens. Accepts a boolean argument which
// specifies whether to return non-captures.
void CBoard::rookGen(mv **moveList, BB pins, bool quietMoves) {
    BB sliders = pieces[wtm][ROOK] | pieces[wtm][QUEEN];
    while(sliders) {
        ind i = bitscan(sliders);
        BB movesBB = magics::rookMoves(i, occupied);
        movesBB &= quietMoves ? ~pieces[wtm][ALL] : pieces[!wtm][ALL];
        serialize(moveList, movesBB, i);
        sliders &= ~exp_2(i);
    }
}

// Returns a bitmap of all squares threatened by a given color on the current board. A piece is
// threatening any square it aims at or can move to, even if it is pinned.
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
    while(b) {
        ind dest = bitscan(b);
        *((*moveList)++) = moves::make(source, dest, board[source], board[dest], NONE, NONE, NONE);
        b ^= exp_2(dest);
    }
}

// serializePawn serializes a move bitboard based on a "delta" as a list of moves. For each set bit
// with index i, creates moves from i-delta to i. Handles pawn promotion moves, but expects double
// pawn moves and en-passant capture special flags to be passed in. Note that promotions and these
// types of moves are disjoint.
void CBoard::serializePawn(mv **moveList, BB b, ind special, int delta) {
    // non-promotion moves
    BB x = b & ~masks::promoRank(wtm);
    while(x) {
        ind i = bitscan(x);
        *((*moveList)++) = moves::make((int)i - delta, i, PAWN, board[i], 0, 0, special);
        x ^= exp_2(i);
    }
    // promotion moves
    x = b & masks::promoRank(wtm);
    while(x) {
        ind i = bitscan(x);
        for(ind j = PROMOTE_QUEEN; j <= PROMOTE_KNIGHT; j++) {
            *((*moveList)++) = moves::make((int)i - delta, i, PAWN, board[i], 0, 0, j);
        }
        x ^= exp_2(i);
    }
}
