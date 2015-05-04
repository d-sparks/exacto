#pragma once
#include <stdint.h>

// Moves are stored as bitstrings. The bits represent (LSB on right):

// [00000000000000000000000000111111]: source square (6 bits)
// [00000000000000000000111111000000]: dest square (6 bits)
// [00000000000000000111000000000000]: attacker (3 bits)
// [00000000000000111000000000000000]: defender (3 bits)
// [00000000001111000000000000000000]: en passant column (4 bits)
// [00000011110000000000000000000000]: castling data (4 bits)
// [00111100000000000000000000000000]: special data (4 bits)

#define REGULAR_MOVE              0
#define PROMOTE_QUEEN             1
#define PROMOTE_ROOK              2
#define PROMOTE_BISHOP            3
#define PROMOTE_KNIGHT            4
#define DOUBLE_PAWN_MOVE_W        5
#define DOUBLE_PAWN_MOVE_B        6
#define EN_PASSANT_CAP_W          7
#define EN_PASSANT_CAP_B          8
#define KINGSIDE_CASTLE_W         9
#define QUEENSIDE_CASTLE_W        10
#define KINGSIDE_CASTLE_B         11
#define QUEENSIDE_CASTLE_B        12
#define REMOVE_KINGSIDE_CASTLING  13
#define REMOVE_QUEENSIDE_CASTLING 14
#define REMOVE_ALL_CASTLING       15

typedef uint32_t mv;
