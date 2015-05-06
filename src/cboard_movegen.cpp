#pragma once
#include "cboard.h"
#include "squares.h"
#include "bb.h"
#include "masks.cpp"
#include "moves.cpp"

using namespace std;

// Generate legal moves given the current state of the board.
void CBoard::moveGen(mv * moveList) {

    // in check? else:
    pawnGen(&moveList);
    pawnCaps(&moveList);

    knightGen(&moveList, true);

}

// Generates all moves for knights
void CBoard::knightGen(mv **moveList, bool quietMoves) {
    BB knights = pieces[wtm][KNIGHT];
    while(knights) {
        ind i = bitscan(knights);
        BB movesBB = masks::KNIGHT_MOVES[i] & (quietMoves? ~pieces[wtm][ALL] : pieces[!wtm][ALL]);
        serialize(moveList, movesBB, i);
        knights &= ~exp_2(i);
    }
}

// Generates the non-capture pawn moves
void CBoard::pawnGen(mv **moveList) {
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
void CBoard::pawnCaps(mv **moveList) {
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

// serialize turns a bitboard into moves.
void CBoard::serialize(mv **moveList, BB b, ind source) {
    BB x = b & ~masks::promoRank(wtm);
    while(x) {
        ind dest = bitscan(x);
        *((*moveList)++) = moves::make(source, dest, board[source], board[dest], NONE, NONE, NONE);
        x ^= exp_2(dest);
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
