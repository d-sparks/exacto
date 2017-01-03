#define _TEST 1

#include "../src/cboard.cpp"
#include <string.h>
#include <map>
#include "../src/bb.cpp"
#include "assert.h"

using namespace std;

// Test setBoard
int testSetBoardDefaultPosition() {
  cout << "Testing setBoard in default position..." << endl;

  map<ind, ind> expectedBoard = {
      {H1, ROOK},   {G1, KNIGHT}, {F1, BISHOP}, {E1, KING},   {D1, QUEEN},
      {C1, BISHOP}, {B1, KNIGHT}, {A1, ROOK},   {H2, PAWN},   {G2, PAWN},
      {F2, PAWN},   {E2, PAWN},   {D2, PAWN},   {C2, PAWN},   {B2, PAWN},
      {A2, PAWN},   {H8, ROOK},   {G8, KNIGHT}, {F8, BISHOP}, {E8, KING},
      {D8, QUEEN},  {C8, BISHOP}, {B8, KNIGHT}, {A8, ROOK},   {H7, PAWN},
      {G7, PAWN},   {F7, PAWN},   {E7, PAWN},   {D7, PAWN},   {C7, PAWN},
      {B7, PAWN},   {A7, PAWN}};

  CBoard board;
  for (ind i = H1; i < totalSquares; i++) {
    ASSERT(board.board[i] == expectedBoard[i],
           "Wrong piece index " + to_string(i));
  }
  ASSERT(board.pieces[BLACK][ALL] == (BB)0xFFFF000000000000,
         "Black occupancy bitboard incorrect");
  ASSERT(board.pieces[BLACK][PAWN] == (BB)0x00FF000000000000,
         "Black pawns bitboard incorrect");
  ASSERT(board.pieces[BLACK][KNIGHT] == (exp_2(G8) | exp_2(B8)),
         "Black knights bitboard incorrect");
  ASSERT(board.pieces[BLACK][BISHOP] == (exp_2(F8) | exp_2(C8)),
         "Black bishops bitboard incorrect");
  ASSERT(board.pieces[BLACK][ROOK] == (exp_2(H8) | exp_2(A8)),
         "Black bishops bitboard incorrect");
  ASSERT(board.pieces[BLACK][QUEEN] == exp_2(D8),
         "Black queens bitboard incorrect");
  ASSERT(board.pieces[BLACK][KING] == exp_2(E8),
         "Black kings bitboard incorrect");
  ASSERT(board.pieces[WHITE][ALL] == (BB)0x000000000000FFFF,
         "White occupancy bitboard incorrect");
  ASSERT(board.pieces[WHITE][PAWN] == (BB)0x000000000000FF00,
         "White pawns bitboard incorrect");
  ASSERT(board.pieces[WHITE][KNIGHT] == (exp_2(G1) | exp_2(B1)),
         "White knights bitboard incorrect");
  ASSERT(board.pieces[WHITE][BISHOP] == (exp_2(F1) | exp_2(C1)),
         "White bishops bitboard incorrect");
  ASSERT(board.pieces[WHITE][ROOK] == (exp_2(H1) | exp_2(A1)),
         "White bishops bitboard incorrect");
  ASSERT(board.pieces[WHITE][QUEEN] == exp_2(D1),
         "White queens bitboard incorrect");
  ASSERT(board.pieces[WHITE][KING] == exp_2(E1),
         "White kings bitboard incorrect");
  ASSERT(board.occupied == (BB)0xFFFF00000000FFFF,
         "Occupancy bitboard incorrect");
  ASSERT(board.empty == (BB)0x0000FFFFFFFF0000, "Empty bitboard incorrect");
  ASSERT(board.wtm, "Should be white to move");
  ASSERT(board.enPassant == (BB)0, "Shouldn't have enPassant square");
  ASSERT(board.castling[BLACK] == (exp_2(G8) | exp_2(C8)),
         "Wrong castling data for black");
  ASSERT(board.castling[WHITE] == (exp_2(G1) | exp_2(C1)),
         "Wrong castling data for white");
  return 1;
}

// Test setBoard's accuracy with en passant squares
int testSetBoardEnPassant() {
  cout << "Testing setBoard: en passant" << endl;
  for (ind i = H1; i < totalSquares; i++) {
    CBoard board("8/8/8/8/8/8/8/8", "w", "KQkq", squares::algebraic[i]);
    ASSERT(board.enPassant == exp_2(i),
           "Bad enPassant loading " + squares::algebraic[i]);
  }
  return 1;
}

// Test setBoard's accuracy with castling data
int testSetBoardCastling() {
  cout << "Testing setBoard: castling" << endl;

  // No castling rights
  CBoard board("8/8/8/8/8/8/8/8", "w", "");
  ASSERT(board.castling[WHITE] == (BB)0, "Castling failed: '-'");
  ASSERT(board.castling[BLACK] == (BB)0, "Castling failed: '-'");

  // Kingside only
  board = CBoard("8/8/8/8/8/8/8/8", "w", "Kk");
  ASSERT(board.castling[WHITE] == exp_2(squares::index("G1")),
         "Castling failed: '-'");
  ASSERT(board.castling[BLACK] == exp_2(squares::index("G8")),
         "Castling failed: '-'");

  // Queenside only
  board = CBoard("8/8/8/8/8/8/8/8", "w", "Qq");
  ASSERT(board.castling[WHITE] == exp_2(squares::index("C1")),
         "Castling failed: '-'");
  ASSERT(board.castling[BLACK] == exp_2(squares::index("C8")),
         "Castling failed: '-'");

  return 1;
}

// Test setBoard's accuracy with color to move
int testSetBoardColorToMove() {
  cout << "Testing setBoard: color to move" << endl;

  // Black to move
  CBoard board("8/8/8/8/8/8/8/8", "b", "");
  ASSERT(!board.wtm, "Should be black to move");

  // Uppercase letters
  board = CBoard("8/8/8/8/8/8/8/8", "W", "");
  ASSERT(board.wtm, "Should be white to move");

  // Uppercase letters, black to move
  board = CBoard("8/8/8/8/8/8/8/8", "B", "");
  ASSERT(!board.wtm, "Should be black to move");

  return 1;
}

int main() {
  int t = 0;

  t += testSetBoardDefaultPosition();
  t += testSetBoardEnPassant();
  t += testSetBoardCastling();
  t += testSetBoardColorToMove();

  cout << endl;
  cout << t << " test(s) OK" << endl;
  return 0;
}
