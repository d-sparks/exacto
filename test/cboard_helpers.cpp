#include "../src/cboard.cpp"
#include <string.h>
#include <map>
#include "../src/bb.cpp"
#include "assert.h"

using namespace std;

// Test squares::index by checking that it is the inverse of the array
// indexToSquare on the
// appropriate domain.
int testSquareToIndex() {
  cout << "Testing squares::index..." << endl;
  for (ind i = 0; i < totalSquares; i++) {
    ASSERT(i == squares::index(squares::algebraic[i]),
           "squares::index failed for i = " + to_string(i));
  }
  return 1;
}

int main() {
  int t = 0;

  t += testSquareToIndex();

  cout << endl;
  cout << t << " test(s) OK" << endl;
  return 0;
}
