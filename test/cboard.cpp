#define _TEST 1

#include "../src/cboard.cpp"
#include <string.h>
#include <map>
#include "../src/inlines.h"
#include "../src/masks.cpp"
#include "assert.h"

using namespace std;

// testMovePiece simply checks that the given pieces get moved on the board.
// Assumes correctness of
// CBoard::operator== and setBoard.
int testMovePiece() {
  cout << "Testing CBoard::movePiece..." << endl;
  CBoard board1, board2;

  board1.movePiece(WHITE, PAWN, E2, E4, exp_2(E2), exp_2(E4));
  board2.setBoard("rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR");
  board1.occupied = board1.pieces[WHITE][ALL] | board1.pieces[BLACK][ALL];
  ASSERT(board1 == board2, "Move gave wrong result");

  board1.movePiece(BLACK, PAWN, E7, E5, exp_2(E7), exp_2(E5));
  board2.setBoard("rnbqkbnr/pppp1ppp/8/4p3/4P3/8/PPPP1PPP/RNBQKBNR");
  board1.occupied = board1.pieces[WHITE][ALL] | board1.pieces[BLACK][ALL];
  ASSERT(board1 == board2, "Move gave wrong result");

  return 1;
}

// testKillPiece checks that piece removal works. Assumes correctness of
// CBoard::operator== and
// setBoard.
int testKillPiece() {
  cout << "Testing CBoard::killPiece..." << endl;
  CBoard board1, board2;

  board1.setBoard("8/8/8/8/4P3/8/8/8");
  board2.setBoard("8/8/8/8/8/8/8/8");
  board1.killPiece(WHITE, PAWN, E4, exp_2(E4));
  board1.occupied = board1.pieces[WHITE][ALL] | board1.pieces[BLACK][ALL];
  ASSERT(board1 == board2, "killPiece failed");

  board1.setBoard("8/8/8/8/4b3/8/8/8");
  board2.setBoard("8/8/8/8/8/8/8/8");
  board1.killPiece(BLACK, BISHOP, E4, exp_2(E4));
  board1.occupied = board1.pieces[WHITE][ALL] | board1.pieces[BLACK][ALL];
  ASSERT(board1 == board2, "killPiece failed");

  return 1;
}

// testMakePiece checks that creating additional pieces works. Assumes
// correctness of
// CBoard::operator== and setBoard.
int testMakePiece() {
  cout << "Testing CBoard::makePiece..." << endl;
  CBoard board1, board2;

  board1.setBoard("8/8/8/8/8/8/8/8");
  board2.setBoard("8/8/8/8/4N3/8/8/8");
  board1.makePiece(WHITE, KNIGHT, E4, exp_2(E4));
  board1.occupied = board1.pieces[WHITE][ALL] | board1.pieces[BLACK][ALL];
  ASSERT(board1 == board2, "makePiece failed");

  board1.setBoard("8/8/8/8/8/8/8/8");
  board2.setBoard("8/8/8/8/4r3/8/8/8");
  board1.makePiece(BLACK, ROOK, E4, exp_2(E4));
  board1.occupied = board1.pieces[WHITE][ALL] | board1.pieces[BLACK][ALL];
  ASSERT(board1 == board2, "makePiece failed");

  return 1;
}

// Tests the removal of queenside castling rights for the player to move.
int testRemoveQueensideCastlingRights() {
  cout << "Testing removeQueensideCastlingRights..." << endl;
  CBoard board;

  board.removeQueensideCastlingRights(board.wtm);
  ASSERT(popcount(board.castling[board.wtm]) == 1, "Did not remove castling");
  ASSERT((board.castling[board.wtm] & masks::FILE[1]) != 0,
         "Removed kingside castling");

  return 1;
}

// Tests the removal of kingside castling rights for the player to move.
int testRemoveKingsideCastlingRights() {
  cout << "Testing removeKingsideCastlingRights..." << endl;
  CBoard board;

  board.removeKingsideCastlingRights(board.wtm);
  ASSERT(popcount(board.castling[board.wtm]) == 1, "Did not remove castling");
  ASSERT((board.castling[board.wtm] & masks::FILE[5]) != 0,
         "Removed queenside castling");

  return 1;
}

int main() {
  int t = 0;

  t += testMovePiece();
  t += testKillPiece();
  t += testMakePiece();
  t += testRemoveQueensideCastlingRights();
  t += testRemoveKingsideCastlingRights();

  cout << endl;
  cout << t << " test(s) OK" << endl;
  return 0;
}
