#pragma once
#include "cgame.h"
#include "chash.h"

class CExacto {
public:
    int16_t evaluate(CGame* game);
    int16_t search(CGame* game, int16_t alpha, int16_t beta, int16_t depth);
    int16_t qsearch(CGame* game, int16_t alpha, int16_t beta);

    // Presumably later the constructor will accept hash table size and parallelism.
    CExacto(CGame initGame);
    ~CExacto();

    CHash hash;
    CGame game;
};
