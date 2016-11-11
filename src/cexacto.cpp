#pragma once
#include "cexacto.h"
#include "cgame.cpp"
#include "cexacto_evaluate.cpp"
#include "cexacto_search.cpp"
#include "chash.cpp"
#include "moves.cpp"
#include "SEE.cpp"

CExacto::CExacto(CGame initGame) {
    game = initGame;
}

CExacto::~CExacto() {
}

void CExacto::go(CGame* game) {
    cout << "Value: " << search(game, -INFNTY, INFNTY, 3, 0) << endl;
    mv move = hash.getSugg(game->hashKey);
    cout << moves::algebraic(move) << endl;
    game->makeMove(&move);
}

void CExacto::sortMoves(CGame* game, mv* moves) {
    mv hashSugg = BOGUS_MOVE;
    if(hash.probe(game->hashKey, 0) > HASH_ALPHA) {
        hashSugg = hash.getSugg(game->hashKey);
    }
    ind numMoves = 0;
    while(moves[numMoves] != NONE) {
        numMoves++;
    }
    int16_t scores[numMoves];
    for(int i = 0; i < numMoves; i++) {
        mv move = moves[i];
        int16_t score = (move == hashSugg)? MATESCORE : SEE::see(game, move);
        int j = i;
        for(; j > 0 && score > scores[j-1]; j--) {
            scores[j] = scores[j - 1];
            moves[j] = moves[j - 1];
        }
        moves[j] = move;
        scores[j] = score;
    }
}
