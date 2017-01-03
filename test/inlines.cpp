#include "../src/inlines.h"
#include <string.h>
#include "../src/bb.cpp"
#include "assert.h"

using namespace std;

// Test the bitscans on each bit index. Don't test the empty bitboard.
int testBitscan() {
  cout << "Testing bitscans..." << endl;
  BB x = 0xFFFFFFFFFFFFFFFF;
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
  BB x = 1;
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
