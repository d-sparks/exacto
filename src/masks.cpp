#pragma once
#include "bb.h"

using namespace std;

namespace masks {
    BB promoRank(bool color) {
        return color? (BB)0x00FF000000000000 : (BB)0x000000000000FF00;
    }
    BB const RANK[8] = {
        0x00000000000000FF, 0x000000000000FF00, 0x0000000000FF0000, 0x00000000FF000000,
        0x000000FF00000000, 0x0000FF0000000000, 0x00FF000000000000, 0xFF00000000000000
    };
    BB const FILE[8] = {
        0x8080808080808080, 0x4040404040404040, 0x2020202020202020, 0x1010101010101010,
        0x0808080808080808, 0x0404040404040404, 0x0202020202020202, 0x0101010101010101
    };
};