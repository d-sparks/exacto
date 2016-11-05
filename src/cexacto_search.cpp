#pragma once
#include "cexacto.h"
#include "cgame.h"

int CExacto::search(CGame* game, int alpha, int beta, int depth) {
    if(hash.probe(game->hashKey, depth) == HASH_EXACT) {
        return hash.getVal(game->hashKey);
    }
    if(depth == 0) {
        return evaluate(game);
    }
    mv mvs[256] = {0};
    int bestScore = -10000;
    mv bestMove = BOGUS_MOVE;
    game->moveGen(mvs, true);
    for(ind i = 0; mvs[i]; i++) {
        mv move = mvs[i];
        game->makeMove(&move);
        int score = -search(game, -beta, -alpha, depth - 1);
        if(score > bestScore) {
            bestScore = score;
            bestMove = move;
        }
        game->unmakeMove(move);
    }
    hash.record(game->hashKey, bestMove, depth, bestScore, HASH_EXACT);
    return bestScore;
}
