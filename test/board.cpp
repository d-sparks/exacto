#define _TEST 1

#include "../src/board.h"

#include <map>

#include "../src/defines.h"
#include "../src/inlines.h"
#include "../src/masks.h"
#include "../src/squares.h"
#include "assert.h"

using namespace exacto;
using namespace std;

// TestMovePiece simply checks that the given pieces get moved on the board.
// Assumes correctness of
// Board::operator== and setBoard.
int TestMovePiece() {
  cout << "Testing Board::MovePiece..." << endl;
  Board board1, board2;

  board1.MovePiece(WHITE, PAWN, E2, E4, exp_2(E2), exp_2(E4));
  board2.setBoard("rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR");
  board1.occupied = board1.pieces[WHITE][ALL] | board1.pieces[BLACK][ALL];
  ASSERT(board1 == board2, "Move gave wrong result");

  board1.MovePiece(BLACK, PAWN, E7, E5, exp_2(E7), exp_2(E5));
  board2.setBoard("rnbqkbnr/pppp1ppp/8/4p3/4P3/8/PPPP1PPP/RNBQKBNR");
  board1.occupied = board1.pieces[WHITE][ALL] | board1.pieces[BLACK][ALL];
  ASSERT(board1 == board2, "Move gave wrong result");

  return 1;
}

// TestKillPiece checks that piece removal works. Assumes correctness of
// Board::operator== and
// setBoard.
int TestKillPiece() {
  cout << "Testing Board::KillPiece..." << endl;
  Board board1, board2;

  board1.setBoard("8/8/8/8/4P3/8/8/8");
  board2.setBoard("8/8/8/8/8/8/8/8");
  board1.KillPiece(WHITE, PAWN, E4, exp_2(E4));
  board1.occupied = board1.pieces[WHITE][ALL] | board1.pieces[BLACK][ALL];
  ASSERT(board1 == board2, "KillPiece failed");

  board1.setBoard("8/8/8/8/4b3/8/8/8");
  board2.setBoard("8/8/8/8/8/8/8/8");
  board1.KillPiece(BLACK, BISHOP, E4, exp_2(E4));
  board1.occupied = board1.pieces[WHITE][ALL] | board1.pieces[BLACK][ALL];
  ASSERT(board1 == board2, "KillPiece failed");

  return 1;
}

// TestMakePiece checks that creating additional pieces works. Assumes
// correctness of Board::operator== and setBoard.
int TestMakePiece() {
  cout << "Testing Board::MakePiece..." << endl;
  Board board1, board2;

  board1.setBoard("8/8/8/8/8/8/8/8");
  board2.setBoard("8/8/8/8/4N3/8/8/8");
  board1.MakePiece(WHITE, KNIGHT, E4, exp_2(E4));
  board1.occupied = board1.pieces[WHITE][ALL] | board1.pieces[BLACK][ALL];
  ASSERT(board1 == board2, "MakePiece failed");

  board1.setBoard("8/8/8/8/8/8/8/8");
  board2.setBoard("8/8/8/8/4r3/8/8/8");
  board1.MakePiece(BLACK, ROOK, E4, exp_2(E4));
  board1.occupied = board1.pieces[WHITE][ALL] | board1.pieces[BLACK][ALL];
  ASSERT(board1 == board2, "MakePiece failed");

  return 1;
}

// Tests the removal of queenside castling rights for the player to move.
int TestRemoveQueensideCastlingRights() {
  cout << "Testing RemoveQueensideCastlingRights..." << endl;
  Board board;

  board.RemoveQueensideCastlingRights(board.wtm);
  ASSERT(popcount(board.castling[board.wtm]) == 1, "Did not remove castling");
  ASSERT((board.castling[board.wtm] & masks::FILE[1]) != 0,
         "Removed kingside castling");

  return 1;
}

// Tests the removal of kingside castling rights for the player to move.
int TestRemoveKingsideCastlingRights() {
  cout << "Testing RemoveKingsideCastlingRights..." << endl;
  Board board;

  board.RemoveKingsideCastlingRights(board.wtm);
  ASSERT(popcount(board.castling[board.wtm]) == 1, "Did not remove castling");
  ASSERT((board.castling[board.wtm] & masks::FILE[5]) != 0,
         "Removed queenside castling");

  return 1;
}

int main() {
  int t = 0;

  t += TestMovePiece();
  t += TestKillPiece();
  t += TestMakePiece();
  t += TestRemoveQueensideCastlingRights();
  t += TestRemoveKingsideCastlingRights();

  cout << endl;
  cout << t << " test(s) OK" << endl;
  return 0;
}
