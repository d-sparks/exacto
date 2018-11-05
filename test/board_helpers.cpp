#include "../src/board.h"

#include <string>
#include <map>

#include "../src/bitboard.h"
#include "../src/defines.h"
#include "../src/inlines.h"
#include "../src/squares.h"
#include "assert.h"

using namespace exacto;
using namespace std;

// Test squares::index by checking that it is the inverse of the array
// indexToSquare on the
// appropriate domain.
int TestSquareToIndex() {
  cout << "Testing squares::index..." << endl;
  for (ind i = 0; i < NUM_SQUARES; ++i) {
    ASSERT(i == squares::index(squares::algebraic[i]),
           "squares::index failed for i = " + to_string(i));
  }
  return 1;
}

int main() {
  int t = 0;

  t += TestSquareToIndex();

  cout << endl;
  cout << t << " test(s) OK" << endl;
  return 0;
}
