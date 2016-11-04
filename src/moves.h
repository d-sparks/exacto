#pragma once
#include <stdint.h>

// Moves are stored as bitstrings. The bits represent (LSB on right):

// [00000000000000000000000000111111]: source square (6 bits)
// [00000000000000000000111111000000]: dest square (6 bits)
// [00000000000000000111000000000000]: attacker (3 bits)
// [00000000000000111000000000000000]: defender (3 bits)
// [00000000001111000000000000000000]: en passant column (4 bits)
// [00000011110000000000000000000000]: castling data (4 bits - KQkq)
// [11111100000000000000000000000000]: special data (6 bits)

#define REGULAR_MOVE              0
#define PROMOTE_KNIGHT            2
#define PROMOTE_BISHOP            3
#define PROMOTE_ROOK              4
#define PROMOTE_QUEEN             5
#define DOUBLE_PAWN_MOVE_W        8
#define DOUBLE_PAWN_MOVE_B        9
#define EN_PASSANT_CAP            10
#define CASTLE                    11
#define KING_MOVE                 12
#define REMOVE_KINGSIDE_CASTLING  13
#define REMOVE_QUEENSIDE_CASTLING 14
#define REMOVE_ALL_CASTLING       15

#define KINGSIDE                  0
#define QUEENSIDE                 1

typedef uint32_t mv;
