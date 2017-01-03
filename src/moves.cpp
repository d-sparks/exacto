#pragma once
#include "moves.h"
#include <iostream>
#include <string>
#include <vector>
#include "bb.h"
#include "cgame.h"
#include "inlines.h"
#include "squares.cpp"

using namespace std;

namespace moves {
ind source(mv m) { return m & 63; }

ind dest(mv m) { return (m >> 6) & 63; }

ind attacker(mv m) { return (m >> 12) & 7; }

ind defender(mv m) { return (m >> 15) & 7; }

ind enPassant(mv m) { return (m >> 18) & 15; }

ind castling(mv m) { return (m >> 22) & 15; }

ind special(mv m) { return (m >> 26); }

mv make(mv source, mv dest, mv attacker, mv defender, mv enPassant, mv castling,
        mv special) {
  return source | (dest << 6) | (attacker << 12) | (defender << 15) |
         (enPassant << 18) | (castling << 22) | (special << 26);
}

string algebraic(mv m) {
  ind promoData = special(m);
  string out = squares::algebraic[source(m)] + squares::algebraic[dest(m)];
  if (promoData == PROMOTE_BISHOP) out += "B";
  if (promoData == PROMOTE_KNIGHT) out += "N";
  if (promoData == PROMOTE_ROOK) out += "R";
  if (promoData == PROMOTE_QUEEN) out += "Q";
  return out;
}

// Castling is encoded into four bits in a move in the 22-25 indices,
// representing KQkq respectively.
mv castlingEncode(BB castlingData) {
  mv out = 0;
  out |= (exp_2(G1) & castlingData) << (22 - G1);
  out |= (exp_2(C1) & castlingData) << (23 - C1);
  out |= (exp_2(G8) & castlingData) >> (G8 - 24);
  out |= (exp_2(C8) & castlingData) >> (C8 - 25);
  return out;
}

// Mutates the given move by setting the relevant bit indicating castling rights
// change.
void encodeKingsideCastlingChange(mv* move, bool color) {
  *move |= mv(1) << 24 - 2 * color;
}

// Mutates the given move by setting the relevant bit indicating castling rights
// change.
void encodeQueensideCastlingChange(mv* move, bool color) {
  *move |= mv(1) << 25 - 2 * color;
}

// Decodes a castlingCode as outputted by castlingEncode.
BB castlingDecode(ind castlingCode) {
  BB out = 0;
  out |= (BB)(1 & castlingCode) << (G1 - 0);
  out |= (BB)(2 & castlingCode) << (C1 - 1);
  out |= (BB)(4 & castlingCode) << (G8 - 2);
  out |= (BB)(8 & castlingCode) << (C8 - 3);
  return out;
}

// Print a move
void print(mv move) { cout << algebraic(move); }

// Create a mv from cin input
mv cinMove(mv* candidateMoves) {
  string moveString;
  cin >> moveString;
  string sourceString = moveString.substr(0, 2);
  string destString = moveString.substr(2, 2);
  ind sourceInd = squares::index(sourceString);
  ind destInd = squares::index(destString);

  vector<mv> matches;
  mv out;

  for (mv* move = candidateMoves; *move; move++) {
    if (sourceInd == source(*move) && destInd == dest(*move)) {
      matches.push_back(*move);
    }
  }

  if (matches.size() == 1) {
    out = matches.at(0);
  } else {
    // determine pawn promotion type
  }
  return out;
}
}
