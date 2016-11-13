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
    post = true;
}

CExacto::~CExacto() {
}

void CExacto::go(CGame* game) {
    // hardcoding checks per second and average time for search
    int seconds = 7;

    // timing initialization
    nodes = 0;
    uint64_t prevNodes = 1;
    double branchingFactor = 3;
    int NPS = 1000000;
    terminateSearch = false;
    int64_t t_0 = clock();
    int64_t t = 0;

    mv bestMove = BOGUS_MOVE;
    for(int depth = 1; true; depth++) {
        prevNodes = nodes;
        int16_t score = search(game, -INFNTY, INFNTY, depth, 0);
        if(depth == 1) {
            continue;
        }

        t = clock() - t_0;
        double secs = double(t) / CLOCKS_PER_SEC;
        branchingFactor = nodes / prevNodes;
        NPS = (nodes * CLOCKS_PER_SEC) / t;

        if(terminateSearch) {
            break;
        }

        bestMove = hash.getSugg(game->hashKey);
        if(post) {
            cout << depth << "\t";
            cout << "Score: " << score << "\t";
            cout << "Move: " << moves::algebraic(bestMove) << "\t";
            cout << "NPS: " << NPS << "\t";
            cout << "Branching: " << branchingFactor << endl;
        }

        if(secs * branchingFactor > seconds) {
            break;
        }
    }

    cout << "move " << moves::algebraic(bestMove) << endl;
    game->makeMove(&bestMove);
}

void CExacto::sortMoves(CGame* game, mv* mvs) {
    mv hashSugg = BOGUS_MOVE;
    if(hash.probe(game->hashKey, 0) > HASH_ALPHA) {
        hashSugg = hash.getSugg(game->hashKey);
    }
    ind numMoves = 0;
    while(mvs[numMoves] != NONE) {
        numMoves++;
    }
    int16_t scores[numMoves];
    for(int i = 0; i < numMoves; i++) {
        mv move = mvs[i];
        int16_t score = (move == hashSugg)? MATESCORE : SEE::see(game, move);
        ind attacker = moves::attacker(move);
        if(attacker <= BISHOP) {
            ind source = moves::source(move);
            ind dest = moves::dest(move);
            if(game->wtm) {
                score += PVT[WHITE][attacker][dest] - PVT[WHITE][attacker][source];
            } else {
                score -= PVT[BLACK][attacker][dest] - PVT[BLACK][attacker][source];
            }
        }
        int j = i;
        for(; j > 0 && score > scores[j-1]; j--) {
            scores[j] = scores[j - 1];
            mvs[j] = mvs[j - 1];
        }
        mvs[j] = move;
        scores[j] = score;
    }
}

void CExacto::sortCaps(CGame* game, mv* moves) {
    int numMoves = 0;
    while(moves[numMoves]) {
        numMoves++;
    }
    int16_t scores[numMoves];
    for(int i = 0; i < numMoves; i++) {
        mv move = moves[i];
        int16_t score = SEE::see(game, move);
        int j = i;
        for(; j > 0 && score > scores[j-1]; j--) {
            scores[j] = scores[j - 1];
            moves[j] = moves[j - 1];
        }
        moves[j] = move;
        scores[j] = score;
    }
}
