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
    mv enPassantFile = 9;
    if(enPassant) {
        enPassantFile = squares::file(bitscan(enPassant));
    }
    *m |= enPassantFile << 18;
    setEnPassant();

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
        if(castling[wtm]) {
            removeKingsideCastlingRights(wtm);
            removeQueensideCastlingRights(wtm);
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

    // Remove castling rights for rook moves
    BB castlingBB = castling[wtm];
    if(castlingBB != 0 && attacker == ROOK) {
        BB kingSide = castlingBB & masks::FILE[1];
        if((kingSide >> 1) & sourceBB) {
            removeKingsideCastlingRights(wtm);
        }
        BB queenSide = castlingBB & masks::FILE[5];
        if((queenSide << 2) & sourceBB) {
            removeQueensideCastlingRights(wtm);
        }
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
    } else {
        enPassant = 0;
    }

    occupied = pieces[WHITE][ALL] | pieces[BLACK][ALL];
}

// Browses the game tree and counts the number of nodes
BB perft(CGame * game, int depth) {
    BB nodes = 1;

    if(depth > 0) {
        mv moves[256];
        game->moveGen(moves);
        for(mv *move = moves; *move; move++) {
            game->makeMove(move);
            nodes += perft(game, depth - 1);
            game->unmakeMove(*move);
        }
    }

    return nodes;
}

// Browse game tree and count nodes, list descendent counts for each child
BB divide(CGame * game, int depth) {
    BB nodes = 0;
    BB descendents = 0;

    mv moves[256];
    int numOfMoves = 0;
    game->moveGen(moves);
    for(mv *move = moves; *move; move++) {
        numOfMoves++;
        game->makeMove(move);
        cout << moves::algebraic(*move) << " ";
        if(depth >= 2) {
            descendents = perft(game, depth - 1) - perft(game, depth - 2);
        } else {
            descendents = 1;
        }
        game->unmakeMove(*move);
        cout << descendents << endl;
        nodes += descendents;
    }

    cout << "total " << nodes << endl;
    cout << "moves " << numOfMoves << endl;

    return nodes;
}

