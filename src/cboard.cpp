#pragma once
#include <string>
#include <map>
#include "cboard.h"
#include "inlines.h"
#include "bb.cpp"

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
    btm = !wtm;

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
        int r = atoi(ep.substr(0, 1).c_str()) - 96;
        int f = atoi(ep.substr(1, 1).c_str()) - 1;
        enPassant = exp_2((8 * f) + r);
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

