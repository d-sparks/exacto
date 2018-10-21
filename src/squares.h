#ifndef exacto_src_squares_h
#define exacto_src_squares_h

#include <algorithm>
#include <string>

#include "bitboard.h"

namespace exacto {

// TODO: do I need this as well as algebraic?
enum algebraic_squares : ind {
  H1 = 0, G1, F1, E1, D1, C1, B1, A1,
  H2, G2, F2, E2, D2, C2, B2, A2,
  H3, G3, F3, E3, D3, C3, B3, A3,
  H4, G4, F4, E4, D4, C4, B4, A4,
  H5, G5, F5, E5, D5, C5, B5, A5,
  H6, G6, F6, E6, D6, C6, B6, A6,
  H7, G7, F7, E7, D7, C7, B7, A7,
  H8, G8, F8, E8, D8, C8, B8, A8,
  NUM_SQUARES
};

namespace squares {

// Returns the rank of the square, e.g. `rank(E4) = 3`.
inline ind rank(ind square) { return square / 8; }

// Returns the file of the square, e.g. `file(H4) = 0`.
inline ind file(ind square) { return square % 8; }

// squares::index turns algebraic square notation into Exacto's index
// representation. E.g., "h1" becomes 0, "g1" becomes 1, etc.
ind index(std::string square);

// The inverse of squares::index.
extern const std::string algebraic[64];

}  // namespace squares

}  // namespace exacto

#endif  // exacto_src_squares_h
