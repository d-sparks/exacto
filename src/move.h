#pragma once
#include <stdint.h>

// Moves are stored as bitstrings. The bits represent (LSB on right):

// [00000000000000000000000000111111]: source square (6 bits)
// [00000000000000000000111111000000]: dest square (6 bits)
// [00000000000111000000000000000000]: attacker (3 bits)
// [00000000000000111000000000000000]: defender (3 bits)
// [00000000001111000000000000000000]: en passant column (4 bits)
// [00000011110000000000000000000000]: castling data (4 bits)
// [00111100000000000000000000000000]: special data (4 bits)

typedef uint32_t mv;
