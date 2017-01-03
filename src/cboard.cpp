#pragma once
#include "cboard.h"
#include <string.h>
#include <map>
#include <string>
#include "bb.cpp"
#include "cboard_movegen.cpp"
#include "inlines.h"
#include "moves.cpp"
#include "squares.cpp"
#include "zobrist_keys.cpp"

using namespace std;

// Constructor and destructor
CBoard::CBoard(string brd, string clr, string cstl, string ep) {
  setBoard(brd, clr, cstl, ep);
}

CBoard::~CBoard() {}

// print prints a graphical representation of the board.
void CBoard::print() {
  bool castlingWK = (castling[WHITE] & exp_2(G1)) != 0;
  bool castlingWQ = (castling[WHITE] & exp_2(C1)) != 0;
  bool castlingBK = (castling[BLACK] & exp_2(G8)) != 0;
  bool castlingBQ = (castling[BLACK] & exp_2(C8)) != 0;

  string piecesToStrings[16] = {"   ", " p ", " n ", " b ", " r ",
                                " q ", "[k]", "",    "   ", " P ",
                                " N ", " B ", " R ", " Q ", "[K]"};
  map<int, string> sideBar = {
      {7, wtm ? "White to move" : "Black to move"},
      {6, "Static evaluation: "},
      {5, "Q-search value: "},
      {4, "Repititions: "},
      {3, "Castling     Kingside/Queenside"},
      {2, (string) "White:    " + "      " + (castlingWK ? "Yes" : "No") +
              "      " + (castlingWQ ? "Yes" : "No")},
      {1, (string) "Black:    " + "      " + (castlingBK ? "Yes" : "No") +
              "      " + (castlingBQ ? "Yes" : "No")},
  };

  cout << endl;
  for (ind i = 7; i < 255; i--) {
    cout << "     +---+---+---+---+---+---+---+---+" << endl;
    cout << "  " << (int)(i + 1) << "  ";
    for (ind j = 7; j < 255; j--) {
      ind square = 8 * i + j;
      ind colorModifier = (pieces[WHITE][ALL] & exp_2(square)) ? 8 : 0;
      cout << "|" << piecesToStrings[colorModifier + board[square]];
    }
    cout << "|     " << sideBar[i] << endl;
  }
  cout << "     +---+---+---+---+---+---+---+---+" << endl;
  cout << "       A   B   C   D   E   F   G   H  " << endl;
}

// setBoard sets board given an FEN string.
void CBoard::setBoard(string brd, string clr, string cstl, string ep) {
  map<string, ind> stringToPiece = {
      {"p", 1}, {"n", 2},  {"b", 3},  {"r", 4},  {"q", 5},  {"k", 6},
      {"P", 9}, {"N", 10}, {"B", 11}, {"R", 12}, {"Q", 13}, {"K", 14},
  };

  // Clear the board first
  memset(pieces, 0, sizeof(pieces[0][0]) * 2 * 7);
  memset(board, 0, sizeof(board[0]) * 64);
  memset(castling, 0, sizeof(castling[0]) * 2);
  hashKey = 0;

  // Parse FEN board and set pieces, board arrays
  for (ind x = 0, y = 63; x < brd.length(); x++, y--) {
    string temp = brd;
    temp = temp.substr(x, 1);
    if (atoi(temp.c_str()) == 0) {
      if (temp == "/") {
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
  if (wtm) {
    hashKey = hashKey ^ zobrist::wtm;
  }

  // Set castling data
  for (ind i = 0; i < cstl.length(); i++) {
    string temp = cstl;
    temp = temp.substr(i, 1);
    if (temp == "K") grantKingsideCastlingRights(WHITE);
    if (temp == "Q") grantQueensideCastlingRights(WHITE);
    if (temp == "k") grantKingsideCastlingRights(BLACK);
    if (temp == "q") grantQueensideCastlingRights(BLACK);
  }

  // Set en passant data
  enPassant = 0;
  if (ep != "-") {
    setEnPassant(squares::index(ep));
  }

  // Set occupancy bitboards
  for (ind i = BLACK; i <= WHITE; i++) {
    for (ind j = PAWN; j <= KING; j++) {
      pieces[i][ALL] |= pieces[i][j];
    }
  }
  occupied = pieces[BLACK][ALL] | pieces[WHITE][ALL];
  empty = ~occupied;
}

// operator== for CBoards.
bool CBoard::operator==(const CBoard &other) const {
  bool equal = true;
  equal &= memcmp(pieces, other.pieces, sizeof(pieces[0][0]) * 2 * 7) == 0;
  equal &= memcmp(board, other.board, sizeof(board[0]) * 64) == 0;
  equal &= memcmp(castling, other.castling, sizeof(castling[0]) * 2) == 0;
  equal &= enPassant == other.enPassant;
  equal &= wtm == other.wtm;
  equal &= occupied == other.occupied;
  equal &= hashKey == other.hashKey;

  return equal;
}

// Moves a piece on the board, assumes target is empty. Does not udpate
// occupancy bitboard.
void CBoard::movePiece(bool color, ind attacker, ind source, ind dest,
                       BB sourceBB, BB destBB) {
  killPiece(color, attacker, source, sourceBB);
  makePiece(color, attacker, dest, destBB);
}

// Assumes the target square is empty. Does not update occupancy bitboard.
void CBoard::makePiece(bool color, ind piece, ind square, BB squareBB) {
  pieces[color][piece] |= squareBB;
  pieces[color][ALL] |= squareBB;
  board[square] = piece;
  hashKey ^= zobrist::pieces[color][piece][square];
}

// Assumes target square is non-empty. Does not update occupancy bitboard.
void CBoard::killPiece(bool color, ind piece, ind square, BB squareBB) {
  pieces[color][piece] &= ~squareBB;
  pieces[color][ALL] &= ~squareBB;
  board[square] = 0;
  hashKey ^= zobrist::pieces[color][piece][square];
}

// Sets the enPassant square.
void CBoard::setEnPassant(ind square) {
  if (enPassant != 0) {
    hashKey ^= zobrist::en_passant[squares::file(bitscan(enPassant))];
  }
  if (square == 64) {
    enPassant = 0;
  } else {
    enPassant = exp_2(square);
    hashKey ^= zobrist::en_passant[squares::file(square)];
  }
}

// Unsets the queenside castling rights for the color to move.
void CBoard::removeQueensideCastlingRights(bool color) {
  if (castling[color] & masks::FILE[5]) {
    hashKey ^= zobrist::castling[color][QUEENSIDE];
    castling[color] &= ~masks::FILE[5];
  }
}

// Unsets the kingside castling rights for the color to move.
void CBoard::removeKingsideCastlingRights(bool color) {
  if (castling[color] & masks::FILE[1]) {
    hashKey ^= zobrist::castling[color][KINGSIDE];
    castling[color] &= ~masks::FILE[1];
  }
}

// Sets the queenside castling rights for the color to move.
void CBoard::grantQueensideCastlingRights(bool color) {
  if (!(castling[color] & masks::FILE[5])) {
    hashKey ^= zobrist::castling[color][QUEENSIDE];
    castling[color] |= exp_2(color ? C1 : C8);
  }
}

// Sets the kingside castling rights for the color to move.
void CBoard::grantKingsideCastlingRights(bool color) {
  if (!(castling[color] & masks::FILE[1])) {
    hashKey ^= zobrist::castling[color][KINGSIDE];
    castling[color] |= exp_2(color ? G1 : G8);
  }
}
