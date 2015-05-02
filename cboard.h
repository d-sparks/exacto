#pragma once
#include "bb.cpp"

using namespace std;

#define WHITE 1
#define BLACK 0

#define ALL 0
#define PAWN 1
#define KNIGHT 2
#define BISHOP 3
#define ROOK 4
#define QUEEN 5
#define KING 6

class CBoard {
public:
    void print();
    void setBoard(string brd, string clr, string cstl, string ep, string hm, string fm);
    CBoard(void);
    ~CBoard(void);
private:
    bool    wtm,
            btm;
    BB      pieces[2][7],
            castling[2],
            enPassant,
            occupied,
            empty;
    ind     board[64];
};
