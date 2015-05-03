#pragma once
#include <map>
#include "bb.h"

#define WHITE 1
#define BLACK 0

#define ALL 0
#define PAWN 1
#define KNIGHT 2
#define BISHOP 3
#define ROOK 4
#define QUEEN 5
#define KING 6

enum squares : ind {
    H1 = 0, G1, F1, E1, D1, C1, B1, A1,
    H2, G2, F2, E2, D2, C2, B2, A2,
    H3, G3, F3, E3, D3, C3, B3, A3,
    H4, G4, F4, E4, D4, C4, B4, A4,
    H5, G5, F5, E5, D5, C5, B5, A5,
    H6, G6, F6, E6, D6, C6, B6, A6,
    H7, G7, F7, E7, D7, C7, B7, A7,
    H8, G8, F8, E8, D8, C8, B8, A8, totalSquares
};

// Convenient inlines
inline ind white(ind piece) {
    return 8 + piece;
}

inline ind black(ind piece) {
    return piece;
}

inline ind piece(ind coloredPiece) {
    return coloredPiece % 8;
}
