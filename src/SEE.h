#pragma once
#include "bb.h"

class CBoard; // to avoid circular dependency.

namespace SEE {
    // prototypes are here because they are friend functions of CBoard
    int16_t next(CBoard* board, int16_t previousVal, ind square);
    void makeMove(CBoard* board, BB sourceBB, ind attacker);
    void unmakeMove(CBoard* board, BB sourceBB, ind attacker);
    ind leastValuableAttackerSquare(CBoard* board, ind square);
}
