#pragma once
#include <iostream>
#include <string>
#include "moves.cpp"
#include "cboard.cpp"
#include "cgame.h"

using namespace std;

// Constructor and destructor
CGame::CGame(string brd, string clr, string cstl, string ep, string hm, string fm)
    : CBoard(brd, clr, cstl, ep) {
}

CGame::~CGame() {
}

// operator== for comparing CGames
bool CGame::operator==(const CGame &other) const {
    bool boardsEqual = CBoard::operator==(other);
    return boardsEqual;
}

// Makes a move, fully updating the gamestate and history.
void CGame::makeMove(mv m) {
    ind source = moves::source(m);
    ind dest = moves::dest(m);
    ind attacker = moves::attacker(m);
    ind defender = moves::defender(m);
    BB sourceBB = exp_2(source);
    BB destBB = exp_2(dest);

    if(defender) {
        killPiece(!wtm, defender, dest, destBB);
    }
    movePiece(wtm, attacker, source, dest, sourceBB, destBB);

    // Set en passant square to 'none'
    if(enPassant) {
        setEnPassant();
    }

    wtm = !wtm;
    occupied = pieces[WHITE][ALL] | pieces[BLACK][ALL];
}


// Fully undoes a move, updating gamestate and history accordingly.
void CGame::unmakeMove(mv m) {
    ind source = moves::source(m);
    ind dest = moves::dest(m);
    ind attacker = moves::attacker(m);
    ind defender = moves::defender(m);
    BB sourceBB = exp_2(source);
    BB destBB = exp_2(dest);

    wtm = !wtm;

    movePiece(wtm, attacker, dest, source, destBB, sourceBB);
    if(defender) {
        makePiece(!wtm, defender, dest, destBB);
    }

    occupied = pieces[WHITE][ALL] | pieces[BLACK][ALL];
}
