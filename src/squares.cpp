#include "squares.h"


namespace exacto {

namespace squares {

// squares::index turns algebraic square notation into Exacto's index
// representation. E.g., "h1" becomes 0, "g1" becomes 1, etc.
ind index(std::string square) {
  transform(square.begin(), square.end(), square.begin(), ::tolower);
  ind r = 8 - (square.c_str()[0] - 96);
  ind f = atoi(square.substr(1, 1).c_str()) - 1;
  return r + (8 * f);
}

// The inverse of squares::index.
const std::string algebraic[64] = {
    "h1", "g1", "f1", "e1", "d1", "c1", "b1", "a1", "h2", "g2", "f2",
    "e2", "d2", "c2", "b2", "a2", "h3", "g3", "f3", "e3", "d3", "c3",
    "b3", "a3", "h4", "g4", "f4", "e4", "d4", "c4", "b4", "a4", "h5",
    "g5", "f5", "e5", "d5", "c5", "b5", "a5", "h6", "g6", "f6", "e6",
    "d6", "c6", "b6", "a6", "h7", "g7", "f7", "e7", "d7", "c7", "b7",
    "a7", "h8", "g8", "f8", "e8", "d8", "c8", "b8", "a8",
};

}  // namespace squares

}  // namespace exacto
