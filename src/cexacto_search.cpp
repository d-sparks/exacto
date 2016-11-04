#pragma once
#include "cexacto.h"
#include "cgame.h"

int CExacto::search(CGame* game, int alpha, int beta, int depth) {
    if(depth == 0) {
        return evaluate(game);
    }
    mv mvs[256] = {0};
    int bestScore = -10000;
    game->moveGen(mvs, true);
    for(ind i = 0; mvs[i]; i++) {
        mv move = mvs[i];
        game->makeMove(&move);
        int score = -search(game, -beta, -alpha, depth - 1);
        if(score > bestScore) {
            bestScore = score;
        }
        game->unmakeMove(move);
    }
    return bestScore;
}