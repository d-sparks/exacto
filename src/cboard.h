#pragma once
#include <string.h>
#include "moves.h"
#include "pieces.h"
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
    void pawnGen(mv ** moveList, BB pins, bool quietMoves);
    void pawnCaps(mv ** moveList, BB pins);
    void pawnGenPinned(mv **moveList, BB pins, ind kingSquare, bool quietMoves);
    void knightGen(mv ** moveList, BB pins, bool quietMoves);
    void bishopGen(mv ** moveList, BB pins, bool quietMoves);
    void bishopGenPinned(mv **moveList, BB pins, ind kingSquare, bool quietMoves);
    void rookGen(mv ** moveList, BB pins, bool quietMoves);
    void rookGenPinned(mv **moveList, BB pins, ind kingSquare, bool quietMoves);
    void kingGen(mv **moveList, ind kingSquare, BB enemyAttacks, bool quietMoves);
    BB attackSetGen(bool color);
    BB bishopPins(ind kingSquare);
    BB rookPins(ind kingSquare);
    void serialize(mv **moveList, BB b, ind source);
    void serialize(mv **moveList, BB b, ind source, ind special);
    void serializeFromDest(mv **moveList, BB b, ind dest, ind defender, ind special);
    void serializePawn(mv **moveList, BB b, ind special, int delta);
    bool    wtm;
    BB      pieces[2][7],
            castling[2],
            enPassant,
            occupied,
            empty;
    ind     board[64];
};
