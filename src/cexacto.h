#pragma once
#include "cgame.h"

class CExacto {
public:
    int evaluate(CGame* game);
    int search(CGame* game, int alpha, int beta, int depth);

    // Presumably later the constructor will accept hash table size and parallelism.
    CExacto(CGame initGame);
    ~CExacto();

    CGame game;
};
