#define _TEST 1

#include <string.h>
#include <map>
#include "assert.h"
#include "../src/cboard.cpp"
#include "../src/bb.cpp"

using namespace std;

// Test setBoard
int testSetBoardDefaultPosition() {
    cout << "Testing setBoard in default position..." << endl;

    map<ind, ind> expectedBoard = {
        {H1, white(ROOK)},  {G1, white(KNIGHT)}, {F1, white(BISHOP)}, {E1, white(KING)},
        {D1, white(QUEEN)}, {C1, white(BISHOP)}, {B1, white(KNIGHT)}, {A1, white(ROOK)},
        {H2, white(PAWN)},  {G2, white(PAWN)},   {F2, white(PAWN)},   {E2, white(PAWN)},
        {D2, white(PAWN)},  {C2, white(PAWN)},   {B2, white(PAWN)},   {A2, white(PAWN)},
        {H8, black(ROOK)},  {G8, black(KNIGHT)}, {F8, black(BISHOP)}, {E8, black(KING)},
        {D8, black(QUEEN)}, {C8, black(BISHOP)}, {B8, black(KNIGHT)}, {A8, black(ROOK)},
        {H7, black(PAWN)},  {G7, black(PAWN)},   {F7, black(PAWN)},   {E7, black(PAWN)},
        {D7, black(PAWN)},  {C7, black(PAWN)},   {B7, black(PAWN)},   {A7, black(PAWN)}
    };

    CBoard board;
    for(ind i = H1; i < totalSquares; i++) {
        ASSERT(board.board[i]          == expectedBoard[i],        "Wrong piece index " + to_string(i));
    }
    ASSERT(board.pieces[BLACK][ALL]    == (BB)0xFFFF000000000000,  "Black occupancy bitboard incorrect");
    ASSERT(board.pieces[BLACK][PAWN]   == (BB)0x00FF000000000000,  "Black pawns bitboard incorrect");
    ASSERT(board.pieces[BLACK][KNIGHT] == (exp_2(G8) | exp_2(B8)), "Black knights bitboard incorrect");
    ASSERT(board.pieces[BLACK][BISHOP] == (exp_2(F8) | exp_2(C8)), "Black bishops bitboard incorrect");
    ASSERT(board.pieces[BLACK][ROOK]   == (exp_2(H8) | exp_2(A8)), "Black bishops bitboard incorrect");
    ASSERT(board.pieces[BLACK][QUEEN]  == exp_2(D8),               "Black queens bitboard incorrect");
    ASSERT(board.pieces[BLACK][KING]   == exp_2(E8),               "Black kings bitboard incorrect");
    ASSERT(board.pieces[WHITE][ALL]    == (BB)0x000000000000FFFF,  "White occupancy bitboard incorrect");
    ASSERT(board.pieces[WHITE][PAWN]   == (BB)0x000000000000FF00,  "White pawns bitboard incorrect");
    ASSERT(board.pieces[WHITE][KNIGHT] == (exp_2(G1) | exp_2(B1)), "White knights bitboard incorrect");
    ASSERT(board.pieces[WHITE][BISHOP] == (exp_2(F1) | exp_2(C1)), "White bishops bitboard incorrect");
    ASSERT(board.pieces[WHITE][ROOK]   == (exp_2(H1) | exp_2(A1)), "White bishops bitboard incorrect");
    ASSERT(board.pieces[WHITE][QUEEN]  == exp_2(D1),               "White queens bitboard incorrect");
    ASSERT(board.pieces[WHITE][KING]   == exp_2(E1),               "White kings bitboard incorrect");
    ASSERT(board.occupied              == (BB)0xFFFF00000000FFFF,  "Occupancy bitboard incorrect");
    ASSERT(board.empty                 == (BB)0x0000FFFFFFFF0000,  "Empty bitboard incorrect");
    ASSERT(board.wtm,                                              "Should be white to move");
    ASSERT(board.enPassant == (BB)0,                               "Shouldn't have enPassant square");
    ASSERT(board.castling[BLACK]       == (exp_2(G8) | exp_2(C8)), "Wrong castling data for black");
    ASSERT(board.castling[WHITE]       == (exp_2(G1) | exp_2(C1)), "Wrong castling data for white");
    return 1;
}

// Test setBoard's accuracy with en passant squares
int testSetBoardEnPassant() {
    cout << "Testing setBoard: en passant" << endl;
    for(ind i = H1; i < totalSquares; i++) {
        CBoard board("8/8/8/8/8/8/8/8", "w", "KQkq", indexToSquare[i], "0", "0");
        ASSERT(board.enPassant == exp_2(i), "Bad enPassant loading " + indexToSquare[i]);
    }
    return 1;
}

// Test setBoard's accuracy with castling data
int testSetBoardCastling() {
    cout << "Testing setBoard: castling" << endl;

    // No castling rights
    CBoard board("8/8/8/8/8/8/8/8", "w", "-", "-", "0", "0");
    ASSERT(board.castling[WHITE] == (BB)0, "Castling failed: '-'");
    ASSERT(board.castling[BLACK] == (BB)0, "Castling failed: '-'");

    // Kingside only
    board = CBoard("8/8/8/8/8/8/8/8", "w", "Kk", "-", "0", "0");
    ASSERT(board.castling[WHITE] == exp_2(squareToIndex("G1")), "Castling failed: '-'");
    ASSERT(board.castling[BLACK] == exp_2(squareToIndex("G8")), "Castling failed: '-'");

    // Queenside only
    board = CBoard("8/8/8/8/8/8/8/8", "w", "Qq", "-", "0", "0");
    ASSERT(board.castling[WHITE] == exp_2(squareToIndex("C1")), "Castling failed: '-'");
    ASSERT(board.castling[BLACK] == exp_2(squareToIndex("C8")), "Castling failed: '-'");

    return 1;
}

// Test setBoard's accuracy with color to move
int testSetBoardColorToMove() {
    cout << "Testing setBoard: color to move" << endl;

    // Black to move
    CBoard board("8/8/8/8/8/8/8/8", "b", "-", "-", "0", "0");
    ASSERT(!board.wtm, "Should be black to move");

    // Uppercase letters
    board = CBoard("8/8/8/8/8/8/8/8", "W", "-", "-", "0", "0");
    ASSERT(board.wtm, "Should be white to move");

    // Uppercase letters, black to move
    board = CBoard("8/8/8/8/8/8/8/8", "B", "-", "-", "0", "0");
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