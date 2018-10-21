#include "../src/inlines.h"
#include <string.h>
#include "../src/bitboard.h"
#include "assert.h"

using namespace exacto;

// Test the bitscans on each bit index. Don't test the empty Bitboard.
int testBitscan() {
  cout << "Testing bitscans..." << endl;
  Bitboard x = 0xFFFFFFFFFFFFFFFF;
  for (ind i = 0; i < 64; i++) {
    ASSERT(bitscan(x << i) == i, "Bitscan for LSB " + to_string(i));
    ASSERT(bitscanReverse(x >> i) == 63 - i,
           "Reverse bitscan for MSB " + to_string(i));
  }
  return 1;
}

// Basically identical to esp_2's current implementation.
int testExp_2() {
  cout << "Testing exp_2..." << endl;
  Bitboard x = 1;
  for (ind i = 0; i < 64; i++) {
    ASSERT(exp_2(i) == (x << i), "exp_2(" + to_string(i) + ")");
  }
  return 1;
}

int main() {
  int t = 0;

  t += testBitscan();
  t += testExp_2();

  cout << endl;
  cout << t << " test(s) OK" << endl;
  return 0;
}
