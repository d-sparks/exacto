#pragma once
#include <string>
#include "cboard_defs.h"
#include "bb.cpp"

using namespace std;

class CBoard {
public:
    void print();
    void setBoard();
    void setBoard(string brd, string clr, string cstl, string ep, string hm, string fm);
    void move(ind source, ind dest);
    bool operator==(const CBoard &other) const;
    CBoard();
    CBoard(string brd, string clr, string cstl, string ep, string hm, string fm);
    ~CBoard();

#ifndef _TEST
private:
#endif
    bool    wtm;
    BB      pieces[2][7],
            castling[2],
            enPassant,
            occupied,
            empty;
    ind     board[64];
};
