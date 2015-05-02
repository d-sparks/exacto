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
    CBoard();
    CBoard(string brd, string clr, string cstl, string ep, string hm, string fm);
    ~CBoard();

#ifndef _TEST
private:
#endif
    bool    wtm,
            btm;
    BB      pieces[2][7],
            castling[2],
            enPassant,
            occupied,
            empty;
    ind     board[64];
};

// Convenient inlines
inline ind white(ind piece) {
    return 8 + piece;
}

inline ind black(ind piece) {
    return piece;
}
