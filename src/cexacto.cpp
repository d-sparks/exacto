#pragma once
#include "cexacto.h"
#include "cgame.cpp"
#include "cexacto_evaluate.cpp"
#include "cexacto_search.cpp"
#include "chash.cpp"
#include "moves.cpp"

CExacto::CExacto(CGame initGame) {
    game = initGame;
}

CExacto::~CExacto() {
}

void CExacto::go(CGame* game) {
    search(game, -10000, 10000, 3);
    mv move = hash.getSugg(game->hashKey);
    std::cout << moves::algebraic(move) << endl;
    game->makeMove(&move);
}
