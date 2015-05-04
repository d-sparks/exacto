#pragma once
#include <string.h>
#include "cboard_defs.h"
#include "moves.h"
#include "bb.h"

using namespace std;

// CBoard contains the state of the board. It's main method is moveGen, so, in principal, it should
// have just enough information to generate the legal moves. It does not contain peripheral
// information like times, moves.history, etc.
class CBoard {
public:
    void print();
    void setBoard();
    void setBoard(string brd, string clr, string cstl, string ep, string hm, string fm);
    void moveGen(mv * moveList);
    void movePiece(bool color, ind attacker, ind source, ind dest, BB sourceBB, BB destBB);
    void makePiece(bool color, ind piece, ind square, BB squareBB);
    void killPiece(bool color, ind piece, ind square, BB squareBB);
    bool operator==(const CBoard &other) const;
    CBoard();
    CBoard(string brd, string clr, string cstl, string ep, string hm, string fm);
    ~CBoard();

#ifndef _TEST
protected:
#endif
    void serializePawn(mv **moveList, BB b, ind special, int delta);
    void serialize(mv **moveList, BB b, ind source);
    void pawnGen(mv ** moveList);
    void pawnCaps(mv ** moveList);
    void knightGen(mv ** moveList, bool quietMoves);
    bool    wtm;
    BB      pieces[2][7],
            castling[2],
            enPassant,
            occupied,
            empty;
    ind     board[64];
};
