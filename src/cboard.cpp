#pragma once
#include <string.h>
#include <map>
#include "cboard.h"
#include "cboard_helpers.cpp"
#include "inlines.h"
#include "bb.cpp"
#include "move.cpp"

using namespace std;

// Constructor(s) and destructor(s)
CBoard::CBoard() {
    setBoard();
}

CBoard::CBoard(string brd, string clr, string cstl, string ep, string hm, string fm) {
    setBoard(brd, clr, cstl, ep, hm, fm);
}

CBoard::~CBoard() {
}

// print prints a graphical representation of the board.
void CBoard::print() {
    string piecesToStrings[16] = {
        "   ", " p ", " n ", " b ", " r ", " q ", "[k]", "",
        "   ", " P ", " N ", " B ", " R ", " Q ", "[K]"
    };
    cout << "+---+---+---+---+---+---+---+---+" << endl;
    for (ind i = 7; i < 255; i--) {
        for (ind j = 7; j < 255; j--) {
            cout << "|" << piecesToStrings[board[8 * i + j]];
        }
        cout << "|" << endl;
        cout << "+---+---+---+---+---+---+---+---+" << endl;
    }
}

// setBoard with no arguments sets the board to the default position
void CBoard::setBoard() {
    setBoard("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR", "w", "KQkq", "-", "0", "0");
}

// setBoard sets board given an FEN string.
void CBoard::setBoard(string brd, string clr, string cstl, string ep, string hm, string fm) {
    map<string, ind> stringToPiece = {
        {"p", 1}, {"n", 2}, {"b", 3}, {"r", 4}, {"q", 5}, {"k", 6},
        {"P", 9}, {"N", 10}, {"B", 11}, {"R", 12}, {"Q", 13}, {"K", 14},
    };

    // Clear the board first
    memset(pieces, 0, sizeof(pieces[0][0]) * 2 * 7);
    memset(board, 0, sizeof(board[0]) * 64);
    memset(castling, 0, sizeof(castling[0]) * 2);

    // Parse FEN board and set pieces, board arrays
    for(ind x = 0, y = 63; x < brd.length(); x++, y--) {
        string temp = brd;
        temp = temp.substr(x, 1);
        if(atoi(temp.c_str()) == 0) {
            if(temp == "/") {
                y++;
            } else {
                ind piece = stringToPiece[temp];
                pieces[piece > 8][piece % 8] |= exp_2(y);
                board[y] = piece;
            }
        } else {
            y -= (atoi(temp.c_str()) - 1);
        }
    }

    // Set color to move
    wtm = (clr == "w" || clr == "W");

    // Set castling data
    for(ind i = 0; i < cstl.length(); i++) {
        string temp = cstl;
        temp = temp.substr(i, 1);
        if(temp == "K") castling[WHITE] |= exp_2(1);
        if(temp == "Q") castling[WHITE] |= exp_2(5);
        if(temp == "k") castling[BLACK] |= exp_2(57);
        if(temp == "q") castling[BLACK] |= exp_2(61);
    }

    // Set en passant data
    if(ep == "-") {
        enPassant = 0;
    } else {
        enPassant = exp_2(squareToIndex(ep));
    }

    // Set occupancy bitboards
    for(ind i = BLACK; i <= WHITE; i++) {
        for(ind j = PAWN; j <= KING; j++) {
            pieces[i][ALL] |= pieces[i][j];
        }
    }
    occupied = pieces[BLACK][ALL] | pieces[WHITE][ALL];
    empty = ~occupied;
}

// == operator for CBoards.
bool CBoard::operator==(const CBoard &other) const {
    bool piecesEqual    = memcmp(pieces, other.pieces, sizeof(pieces[0][0]) * 2 * 7) == 0;
    bool boardsEqual    = memcmp(board, other.board, sizeof(board[0]) * 64) == 0;
    bool enPassantEqual = enPassant == other.enPassant;
    bool wtmEqual       = wtm == other.wtm;
    return piecesEqual && boardsEqual && enPassantEqual && wtmEqual;
}

// Makes a move, fully updating the gamestate.
void CBoard::makeMove(mv m) {
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

// Moves a piece on the board, assumes target is empty. Does not udpate occupancy bitboard.
void CBoard::movePiece(bool color, ind attacker, ind source, ind dest, BB sourceBB, BB destBB) {
    killPiece(color, attacker, source, sourceBB);
    makePiece(color, attacker, dest, destBB);
}

// Assumes the target square is empty. Does not update occupancy bitboard.
void CBoard::makePiece(bool color, ind piece, ind square, BB squareBB) {
    pieces[color][piece] |= squareBB;
    pieces[color][ALL] |= squareBB;
    board[square] = color? piece + 8 : piece;
}

// Assumes target square is non-empty. Does not update occupancy bitboard.
void CBoard::killPiece(bool color, ind piece, ind square, BB squareBB) {
    pieces[color][piece] ^= squareBB;
    pieces[color][ALL] ^= squareBB;
    board[square] = 0;
}
