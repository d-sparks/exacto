#include "board.h"

#include <iostream>
#include <cstring>
#include <string>
#include <map>

#include "bitboard.h"
#include "inlines.h"
#include "masks.h"
#include "moves.h"
#include "squares.h"
#include "zobrist_keys.h"

namespace exacto {

Board::Board(const std::string& brd,
             const std::string& clr,
             const std::string& cstl,
             const std::string& ep) {
  SetBoard(brd, clr, cstl, ep);
}

// print prints a graphical representation of the board.
void Board::Print() {
  Print(board_debug());
}

void Board::Print(const std::vector<std::string>& side_bar) {
  static const std::string pieces_to_strings[16] =
      {"   ", " p ", " n ", " b ", " r ", " q ", "[k]", "",    "   ", " P ",
       " N ", " B ", " R ", " Q ", "[K]"};
  std::cout << std::endl;
  int side_bar_counter = 0;
  for (ind i = 7; i < 255; --i) {
    std::cout << "     +---+---+---+---+---+---+---+---+";
    if (i < 7 && side_bar_counter < (int)side_bar.size()) {
      std::cout << "    " << side_bar[side_bar_counter++];
    }
    std::cout << std::endl << "  " << (int)(i + 1) << "  ";
    for (ind j = 7; j < 255; --j) {
      ind square = 8 * i + j;
      ind color_modifier = (pieces[WHITE][ALL] & exp_2(square)) ? 8 : 0;
      std::cout << "|" << pieces_to_strings[color_modifier + board[square]];
    }
    std::cout << "|";
    if (side_bar_counter < (int)side_bar.size()) {
      std::cout << "    " << side_bar[side_bar_counter++];
    }
    std::cout << std::endl;
  }
  std::cout << "     +---+---+---+---+---+---+---+---+" << std::endl;
  std::cout << "       A   B   C   D   E   F   G   H  " << std::endl;
}

std::vector<std::string> Board::board_debug() {
  std::string castling_string = "Castling:\t";
  if (castling[WHITE] & exp_2(G1)) castling_string += (std::string)"K";
  if (castling[WHITE] & exp_2(C1)) castling_string += (std::string)"Q";
  if (castling[BLACK] & exp_2(G8)) castling_string += (std::string)"k";
  if (castling[BLACK] & exp_2(G8)) castling_string += (std::string)"q";
  if (castling_string.length() == 11) castling_string += "-";
  std::vector<std::string> output{(std::string)"To move:\t" + (wtm ? "W" : "B"),
                                  castling_string};
  return output;
}

// SetBoard sets board given an FEN std::string.
void Board::SetBoard(const std::string& brd,
                     const std::string& clr,
                     const std::string& cstl,
                     const std::string& ep) {
  std::map<std::string, ind> string_to_piece = {
      {"p", 1}, {"n", 2},  {"b", 3},  {"r", 4},  {"q", 5},  {"k", 6},
      {"P", 9}, {"N", 10}, {"B", 11}, {"R", 12}, {"Q", 13}, {"K", 14},
  };

  // Clear the board first
  std::memset(pieces, 0, sizeof(pieces[0][0]) * 2 * 7);
  std::memset(board, 0, sizeof(board[0]) * 64);
  std::memset(castling, 0, sizeof(castling[0]) * 2);
  hash_key = 0;

  // Parse FEN board and set pieces, board arrays
  for (ind x = 0, y = 63; x < brd.length(); ++x, --y) {
    std::string temp = brd;
    temp = temp.substr(x, 1);
    if (atoi(temp.c_str()) == 0) {
      if (temp == "/") {
        y++;
      } else {
        ind color = string_to_piece[temp] > 8;
        ind piece = string_to_piece[temp] % 8;
        MakePiece(color, piece, y, exp_2(y));
      }
    } else {
      y -= (atoi(temp.c_str()) - 1);
    }
  }
  // Set color to move
  wtm = (clr == "w" || clr == "W");
  if (wtm) {
    hash_key = hash_key ^ zobrist::wtm;
  }

  // Set castling data
  for (ind i = 0; i < cstl.length(); ++i) {
    std::string temp = cstl;
    temp = temp.substr(i, 1);
    if (temp == "K") GrantKingsideCastlingRights(WHITE);
    if (temp == "Q") GrantQueensideCastlingRights(WHITE);
    if (temp == "k") GrantKingsideCastlingRights(BLACK);
    if (temp == "q") GrantQueensideCastlingRights(BLACK);
  }

  // Set en passant data
  en_passant = 0;
  if (ep != "-") {
    set_en_passant(squares::index(ep));
  }

  // Set occupancy Bitboards
  for (ind i = BLACK; i <= WHITE; ++i) {
    for (ind j = PAWN; j <= KING; ++j) {
      pieces[i][ALL] |= pieces[i][j];
    }
  }
  occupied = pieces[BLACK][ALL] | pieces[WHITE][ALL];
  empty = ~occupied;
}

// operator== for Boards.
bool Board::operator==(const Board &other) const {
  bool equal = true;
  equal &= memcmp(pieces, other.pieces, sizeof(pieces[0][0]) * 2 * 7) == 0;
  equal &= memcmp(board, other.board, sizeof(board[0]) * 64) == 0;
  equal &= memcmp(castling, other.castling, sizeof(castling[0]) * 2) == 0;
  equal &= en_passant == other.en_passant;
  equal &= wtm == other.wtm;
  equal &= occupied == other.occupied;
  equal &= hash_key == other.hash_key;

  return equal;
}

// Moves a piece on the board, assumes target is empty. Does not udpate
// occupancy Bitboard.
void Board::MovePiece(bool color,
                      ind attacker,
                      ind source,
                      ind dest,
                      Bitboard source_bb,
                      Bitboard dest_bb) {
  KillPiece(color, attacker, source, source_bb);
  MakePiece(color, attacker, dest, dest_bb);
}

// Assumes the target square is empty. Does not update occupancy Bitboard.
void Board::MakePiece(bool color,
                      ind piece,
                      ind square,
                      Bitboard square_bb) {
  pieces[color][piece] |= square_bb;
  pieces[color][ALL] |= square_bb;
  board[square] = piece;
  hash_key ^= zobrist::pieces[color][piece][square];
}

// Assumes target square is non-empty. Does not update occupancy Bitboard.
void Board::KillPiece(bool color,
                      ind piece,
                      ind square,
                      Bitboard square_bb) {
  pieces[color][piece] &= ~square_bb;
  pieces[color][ALL] &= ~square_bb;
  board[square] = 0;
  hash_key ^= zobrist::pieces[color][piece][square];
}

// Sets the en_passant square.
void Board::set_en_passant(ind square) {
  if (en_passant != 0) {
    hash_key ^= zobrist::en_passant[squares::file(bitscan(en_passant))];
  }
  if (square == 64) {
    en_passant = 0;
  } else {
    en_passant = exp_2(square);
    hash_key ^= zobrist::en_passant[squares::file(square)];
  }
}

// Unsets the queenside castling rights for the color to move.
void Board::RemoveQueensideCastlingRights(bool color) {
  if (castling[color] & masks::FILE[5]) {
    hash_key ^= zobrist::castling[color][QUEENSIDE];
    castling[color] &= ~masks::FILE[5];
  }
}

// Unsets the kingside castling rights for the color to move.
void Board::RemoveKingsideCastlingRights(bool color) {
  if (castling[color] & masks::FILE[1]) {
    hash_key ^= zobrist::castling[color][KINGSIDE];
    castling[color] &= ~masks::FILE[1];
  }
}

// Sets the queenside castling rights for the color to move.
void Board::GrantQueensideCastlingRights(bool color) {
  if (!(castling[color] & masks::FILE[5])) {
    hash_key ^= zobrist::castling[color][QUEENSIDE];
    castling[color] |= exp_2(color ? C1 : C8);
  }
}

// Sets the kingside castling rights for the color to move.
void Board::GrantKingsideCastlingRights(bool color) {
  if (!(castling[color] & masks::FILE[1])) {
    hash_key ^= zobrist::castling[color][KINGSIDE];
    castling[color] |= exp_2(color ? G1 : G8);
  }
}

}  // namespace exacto
