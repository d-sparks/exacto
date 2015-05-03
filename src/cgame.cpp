#pragma once
#include <iostream>
#include <string>
#include "move.cpp"
#include "cboard.cpp"
#include "cgame.h"

using namespace std;

CGame::CGame() {
}

CGame::CGame(string brd, string clr, string cstl, string ep, string hm, string fm)
    : CBoard(brd, clr, cstl, ep, hm, fm) {
}

CGame::~CGame() {
}

// Makes a move, fully updating the gamestate and history.
void CGame::makeMove(mv m) {
    ind source = move::source(m);
    ind dest = move::dest(m);
    ind attacker = move::attacker(m);
    ind defender = move::defender(m);
    BB sourceBB = exp_2(source);
    BB destBB = exp_2(dest);

    if(defender) {
        killPiece(!wtm, defender, dest, destBB);
    }
    movePiece(wtm, source, dest, attacker, sourceBB, destBB);
}

// Fully undoes a move, updating gamestate and history accordingly.
void CGame::unmakeMove(mv m) {

}
