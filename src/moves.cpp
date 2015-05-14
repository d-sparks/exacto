#pragma once
#include <iostream>
#include <string.h>
#include "bb.h"
#include "moves.h"
#include "squares.cpp"

using namespace std;

namespace moves {
    ind source(mv m) {
        return m & 63;
    }

    ind dest(mv m) {
        return (m >> 6) & 63;
    }

    ind attacker(mv m) {
        return (m >> 12) & 7;
    }

    ind defender(mv m) {
        return (m >> 15) & 7;
    }

    ind enPassant(mv m) {
        return (m >> 18) & 15;
    }

    ind castling(mv m) {
        return (m >> 22) & 15;
    }

    ind special(mv m) {
        return (m >> 26);
    }

    mv make(mv source, mv dest, mv attacker, mv defender, mv enPassant, mv castling, mv special) {
        return source | (dest << 6)
                      | (attacker << 12)
                      | (defender << 15)
                      | (enPassant << 18)
                      | (castling << 22)
                      | (special << 26);
    }

    string algebraic(mv m) {
        return squares::algebraic[source(m)] + "-" + squares::algebraic[dest(m)] + " " + to_string(defender(m));
    }

    mv castlingEncode(BB castlingData) {
        mv out = 0;
        out |= (BB)G1 << (22 - G1);
        out |= (BB)C1 << (23 - C1);
        out |= (BB)G8 >> (G8 - 24);
        out |= (BB)C8 << (C8 - 25);
        return out;
    }
}
