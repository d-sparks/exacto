#pragma once
#include <string.h>
#include <map>
#include "cboard.h"
#include "inlines.h"
#include "moves.cpp"
#include "bb.cpp"
#include "cboard_movegen.cpp"
#include "squares.cpp"

using namespace std;

// Constructor and destructor
CBoard::CBoard(string brd, string clr, string cstl, string ep) {
    setBoard(brd, clr, cstl, ep);
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
            ind square = 8 * i + j;
            ind colorModifier = (pieces[WHITE][ALL] & exp_2(square))? 8 : 0;
            cout << "|" << piecesToStrings[colorModifier + board[square]];
        }
        cout << "|" << endl;
        cout << "+---+---+---+---+---+---+---+---+" << endl;
    }
}

// setBoard sets board given an FEN string.
void CBoard::setBoard(string brd, string clr, string cstl, string ep) {
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
                ind color = stringToPiece[temp] > 8;
                ind piece = stringToPiece[temp] % 8;
                makePiece(color, piece, y, exp_2(y));
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
        if(temp == "K") castling[WHITE] |= exp_2(G1);
        if(temp == "Q") castling[WHITE] |= exp_2(C1);
        if(temp == "k") castling[BLACK] |= exp_2(G8);
        if(temp == "q") castling[BLACK] |= exp_2(C8);
    }

    // Set en passant data
    enPassant = 0;
    if(ep != "-") {
        setEnPassant(squares::index(ep));
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

// operator== for CBoards.
bool CBoard::operator==(const CBoard &other) const {
    bool piecesEq = memcmp(pieces, other.pieces, sizeof(pieces[0][0]) * 2 * 7) == 0;
    bool boardEq = memcmp(board, other.board, sizeof(board[0]) * 64) == 0;
    bool castlingEq = memcmp(castling, other.castling, sizeof(castling[0]) * 2) == 0;
    bool enPassantEq = enPassant == other.enPassant;
    bool wtmEq = wtm == other.wtm;
    bool occupiedEq = occupied == other.occupied;

    #if _TEST == 2
    if(!piecesEq) cout << "Pieces unequal" << endl;
    if(!boardEq) cout << "Boards unequal" << endl;
    if(!castlingEq) cout << "Castling unequal" << endl;
    if(!enPassantEq) cout << "En passant unequal" << endl;
    if(!wtmEq) cout << "White-to-move unequal" << endl;
    if(!occupiedEq) cout << "Occupied unequal" << endl;
    #endif

    return piecesEq && boardEq && castlingEq && enPassantEq && wtmEq && occupiedEq;
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
    board[square] = piece;
}

// Assumes target square is non-empty. Does not update occupancy bitboard.
void CBoard::killPiece(bool color, ind piece, ind square, BB squareBB) {
    pieces[color][piece] &= ~squareBB;
    pieces[color][ALL] &= ~squareBB;
    board[square] = 0;
}

// Sets the enPassant square.
void CBoard::setEnPassant(ind square) {
    enPassant = (square == 64) ? 0 : exp_2(square);
}

// Unsets the queenside castling rights for the color to move.
void CBoard::removeQueensideCastlingRights(bool color) {
    castling[color] &= ~masks::FILE[5];
}

// Unsets the kingside castling rights for the color to move.
void CBoard::removeKingsideCastlingRights(bool color) {
    castling[color] &= ~masks::FILE[1];
}

