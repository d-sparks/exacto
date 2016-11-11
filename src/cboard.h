#pragma once
#include <string.h>
#include "moves.h"
#include "pieces.h"
#include "bb.h"
#include "SEE.h"

using namespace std;

// CBoard contains the state of the board. It's main method is moveGen, so, in principal, it should
// have just enough information to generate the legal moves. It does not contain peripheral
// information like times, moves.history, etc.
class CBoard {
public:
    // For debugging
    void print();

    // For SEE
    friend int16_t SEE::next(CBoard* board, int16_t previousVal, ind square);
    friend void SEE::makeMove(CBoard* board, BB sourceBB, ind attacker);
    friend void SEE::unmakeMove(CBoard* board, BB sourceBB, ind attacker);
    friend ind SEE::leastValuableAttackerSquare(CBoard* board, ind square);

    // The interface
    void setBoard(
        string brd="rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR",
        string clr="w",
        string cstl="KQkq",
        string ep="-"
    );
    void moveGen(mv * moveList);
    void capGen(mv * moveList);
    void moveGen(mv * moveList, bool quietMoves);
    void movePiece(bool color, ind attacker, ind source, ind dest, BB sourceBB, BB destBB);
    void makePiece(bool color, ind piece, ind square, BB squareBB);
    void killPiece(bool color, ind piece, ind square, BB squareBB);
    void setEnPassant(ind square=64);
    void removeQueensideCastlingRights(bool color);
    void removeKingsideCastlingRights(bool color);
    void grantQueensideCastlingRights(bool color);
    void grantKingsideCastlingRights(bool color);
    bool inCheck();
    bool operator==(const CBoard &other) const;

    uint64_t hashKey;

    // Constructor and destructor
    CBoard(
        string brd="rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR",
        string clr="w",
        string cstl="KQkq",
        string ep="-"
    );
    ~CBoard();

#ifndef _TEST
protected:
#endif
    // Move generation helpers functions. Despite being protected, these methods are unit tested due
    // to their complexity.
    void pawnGen(mv ** moveList, BB pins, bool quietMoves);
    void pawnCaps(mv ** moveList, BB pins);
    void pawnGenPinned(mv **moveList, BB pins, ind kingSquare, bool quietMoves);
    void knightGen(mv ** moveList, BB pins, bool quietMoves);
    void bishopGen(mv ** moveList, BB pins, bool quietMoves);
    void bishopGenPinned(mv **moveList, BB pins, ind kingSquare, bool quietMoves);
    void rookGen(mv ** moveList, BB pins, bool quietMoves);
    void rookGenPinned(mv **moveList, BB pins, ind kingSquare, bool quietMoves);
    void kingGen(mv **moveList, ind kingSquare, BB enemyAttacks, bool quietMoves);
    void generateMovesTo(mv **moveList, ind square, ind defender, BB pins, BB enemyAttacks);
    void evasionGen(mv **moveList, BB enemyAttacks, BB pins, ind kingSquare);
    BB attackSetGen(bool color);
    BB bishopPins(ind kingSquare);
    BB rookPins(ind kingSquare);
    void serialize(mv **moveList, BB b, ind source);
    void serialize(mv **moveList, BB b, ind source, ind special);
    void serializeFromDest(mv **moveList, BB b, ind dest, ind defender, ind special);
    void serializePawn(mv **moveList, BB b, ind special, int delta);
    void closeMoveList(mv **moveList);

    // The state of the board.
    bool wtm;
    BB pieces[2][7];
    BB castling[2];
    BB enPassant;
    BB occupied;
    BB empty;
    ind board[64];
};
