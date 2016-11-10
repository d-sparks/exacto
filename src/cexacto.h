#pragma once
#include "cgame.h"
#include "chash.h"

class CExacto {
public:
    void go(CGame* game);

    int16_t evaluate(CGame* game);
    bool drawnByRepitionOr50MoveRule(CGame* game);
    int16_t search(CGame* game, int16_t alpha, int16_t beta, int16_t depth, int16_t ply);
    int16_t qsearch(CGame* game, int16_t alpha, int16_t beta, int16_t ply);

    // Presumably later the constructor will accept hash table size and parallelism.
    CExacto(CGame initGame);
    ~CExacto();

    CHash hash;
    CGame game;
};
