#pragma once
#include "bb.h"
#include <stdint.h>
#include <bitset>
#include <iostream>
#include "inlines.h"

using namespace std;

namespace bb {

// print prints a bitboard as an 8x8 square of binary. The 0-index bit is in the
// lower-right corner while the 63-index bit is in the upper-left.
void print(BB b) {
  for (int i = 7; i >= 0; i--) {
    cout << (bitset<8>)(b >> (8 * i)) << endl;
  }
}
}
