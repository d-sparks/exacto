#include "bitboard.h"

#include <bitset>
#include <iostream>

#include "inlines.h"

namespace bitboard {

// print prints a Bitboard as an 8x8 square of binary. The 0-index bit is in the
// lower-right corner while the 63-index bit is in the upper-left.
void print(Bitboard b) {
  for (int i = 7; i >= 0; --i) {
    std::cout << (std::bitset<8>)(b >> (8 * i)) << std::endl;
  }
}

}  // namespace bb
