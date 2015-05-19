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
void CGame::makeMove(mv * m) {
    ind source = moves::source(*m);
    ind dest = moves::dest(*m);
    ind attacker = moves::attacker(*m);
    ind defender = moves::defender(*m);
    ind special = moves::special(*m);
    BB sourceBB = exp_2(source);
    BB destBB = exp_2(dest);

    // Move the actual piece
    if(defender && special != EN_PASSANT_CAP) {
        killPiece(!wtm, defender, dest, destBB);
    }
    movePiece(wtm, attacker, source, dest, sourceBB, destBB);

    // Set en passant square to 'none'
    if(enPassant) {
        *m |= (BB)squares::file(bitscan(enPassant)) << 18;
        setEnPassant();
    }

    // Record castling data
    *m |= moves::castlingEncode(castling[wtm]);

    // Special move stuff
    switch(special) {
    case EN_PASSANT_CAP: {
        ind hangingPawn = wtm ? dest - 8 : dest + 8;
        killPiece(!wtm, PAWN, hangingPawn, exp_2(hangingPawn));
        break;
    }
    case DOUBLE_PAWN_MOVE_W:
        setEnPassant(dest - 8);
        break;
    case DOUBLE_PAWN_MOVE_B:
        setEnPassant(dest + 8);
        break;
    case CASTLE: {
        ind rookSource = dest > source ? dest + 2 : dest - 1;
        ind rookDest = dest > source ? dest - 1 : dest + 1;
        movePiece(wtm, ROOK, rookSource, rookDest, exp_2(rookSource), exp_2(rookDest));
    }
    case KING_MOVE: {
        // kill castling rights
        if(castling[wtm]) {
            removeKingsideCastlingRights();
            removeQueensideCastlingRights();
        }
        break;
    }
    case PROMOTE_QUEEN:
    case PROMOTE_ROOK:
    case PROMOTE_BISHOP:
    case PROMOTE_KNIGHT:
        killPiece(wtm, PAWN, dest, destBB);
        makePiece(wtm, special, dest, destBB);
        break;
    }

    wtm = !wtm;
    occupied = pieces[WHITE][ALL] | pieces[BLACK][ALL];
}


// Fully undoes a move, updating gamestate and history accordingly.
void CGame::unmakeMove(mv m) {
    ind source = moves::source(m);
    ind dest = moves::dest(m);
    ind attacker = moves::attacker(m);
    ind special = moves::special(m);
    ind defender = moves::defender(m);
    BB sourceBB = exp_2(source);
    BB destBB = exp_2(dest);

    wtm = !wtm;

    // Special move stuff
    switch(special) {
    case EN_PASSANT_CAP: {
        ind hangingPawn = wtm ? dest - 8 : dest + 8;
        makePiece(!wtm, PAWN, hangingPawn, exp_2(hangingPawn));
        break;
    }
    case CASTLE: {
        ind rookSource = dest > source ? dest + 2 : dest - 1;
        ind rookDest = dest > source ? dest - 1 : dest + 1;
        movePiece(wtm, ROOK, rookDest, rookSource, exp_2(rookDest), exp_2(rookSource));
        break;
    }
    case PROMOTE_QUEEN:
    case PROMOTE_ROOK:
    case PROMOTE_BISHOP:
    case PROMOTE_KNIGHT:
        killPiece(wtm, special, dest, destBB);
        makePiece(wtm, PAWN, dest, destBB);
        break;

    }

    // Move the actual piece
    movePiece(wtm, attacker, dest, source, destBB, sourceBB);
    if(defender && special != EN_PASSANT_CAP) {
        makePiece(!wtm, defender, dest, destBB);
    }

    // Restore castling rights
    castling[wtm] = moves::castlingDecode(moves::castling(m));

    // Reset en passant square
    ind enPassantFile = moves::enPassant(m);
    if(enPassantFile < 9) {
        ind enPassantRank = wtm ? 5 : 2;
        enPassant = exp_2(8 * enPassantRank + enPassantFile);
    }

    occupied = pieces[WHITE][ALL] | pieces[BLACK][ALL];
}
