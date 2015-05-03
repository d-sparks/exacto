#pragma once
#include <string.h>
#include "cboard_defs.h"
#include "bb.cpp"
#include "move.h"

using namespace std;

class CBoard {
public:
    void print();
    void setBoard();
    void setBoard(string brd, string clr, string cstl, string ep, string hm, string fm);
    void makeMove(mv m);
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
    bool    wtm;
    BB      pieces[2][7],
            castling[2],
            enPassant,
            occupied,
            empty;
    ind     board[64];
};
