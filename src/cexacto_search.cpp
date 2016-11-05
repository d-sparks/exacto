#pragma once
#include "cexacto.h"
#include "cgame.h"

int16_t CExacto::search(CGame* game, int16_t alpha, int16_t beta, int16_t depth) {
    // Probe the transposition table to see if we can simply return its value.
    uint8_t hashLookup = hash.probe(game->hashKey, depth);
    if(hashLookup) {
        int16_t hashVal = hash.getVal(game->hashKey);
        if(hashLookup == HASH_EXACT) {
            return hashVal;
        }
        if(hashLookup == HASH_BETA && hashVal >= beta) {
            return hashVal;
        }
        if(hashLookup == HASH_ALPHA && hashVal < alpha) {
            return hashVal;
        }
    }

    // At depth 0, return the static evaluation of the position
    if(depth == 0) {
        return qsearch(game, alpha, beta);
    }

    // Calculate children nodes
    mv mvs[256] = {0};
    int bestScore = alpha;
    mv bestMove = BOGUS_MOVE;
    game->moveGen(mvs);

    // PVS algorithm: iterate over each child, recurse.
    for(ind i = 0; mvs[i]; i++) {
        mv move = mvs[i];
        game->makeMove(&move);
        int score = -search(game, -beta, -alpha, depth - 1);
        game->unmakeMove(move);

        // If score >= beta, the opponent has a better move than the one they played, so we can stop
        // searching. We note in the transposition table that we only have a lower bound on the
        // score of this node. (Fail high.)
        if(score >= beta) {
            hash.record(game->hashKey, bestMove, depth, score, HASH_BETA);
            return score;
        }

        if(score > bestScore) {
            bestScore = score;
            bestMove = move;
        }
    }

    // If bestScore == alpha, no move improved alpha, meaning we can't be sure a descendent didn't
    // fail high, so we note that we only know an upper bound on the score for this node. (Fail
    // low.)
    if(bestScore == alpha) {
        // Fail low: the result isn't precise because a descendent failed high.
        hash.record(game->hashKey, bestMove, depth, alpha, HASH_ALPHA);
        return alpha;
    }

    hash.record(game->hashKey, bestMove, depth, bestScore, HASH_EXACT);
    return bestScore;
}

int16_t CExacto::qsearch(CGame* game, int16_t alpha, int16_t beta) {
    int16_t score = evaluate(game);
    if(score >= beta) {
        return score;
    }
    if(alpha < score) {
        alpha = score;
    }

    // If in check, search all evasions. Otherwise, only loud moves.
    mv mvs[256] = {0};
    game->moveGen(mvs, game->inCheck());

    for(ind i = 0; mvs[i]; i++) {
        mv move = mvs[i];
        game->makeMove(&move);
        int score = -qsearch(game, -beta, -alpha);
        game->unmakeMove(move);
        if(score >= beta) {
            return score;
        }
        if(score > alpha) {
            alpha = score;
        }
    }

    return alpha;
}
