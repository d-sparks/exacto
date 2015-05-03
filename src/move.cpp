#pragma once
#include <iostream>
#include "bb.h"
#include "move.h"

using namespace std;

namespace move {
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
}
