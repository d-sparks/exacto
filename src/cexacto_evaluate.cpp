#include "cexacto_evaluate.h"
#include "cexacto.h"
#include "cgame.h"

int16_t CExacto::evaluate(CGame* game) {
    int16_t score = 0;

    for(ind color = BLACK; color <= WHITE; color++) {
        for(ind piece = PAWN; piece <= KING; piece++) {
            if(piece == ROOK || piece == QUEEN) {
                continue;
            }
            BB pieceboard = game->pieces[color][piece];
            for(ind sq = bitscan(pieceboard); pieceboard; sq = bitscan(pieceboard)) {
                score += PVT[color][piece][sq];
                pieceboard ^= exp_2(sq);
            }
        }
    }

    score += popcount(game->pieces[WHITE][ROOK]) * ROOK_VAL;
    score += popcount(game->pieces[WHITE][QUEEN]) * QUEEN_VAL;
    score -= popcount(game->pieces[BLACK][ROOK]) * ROOK_VAL;
    score -= popcount(game->pieces[BLACK][QUEEN]) * QUEEN_VAL;

    return game->wtm? score : -score;
}

bool CExacto::drawnByRepitionOr50MoveRule(CGame* game) {
    if(game->halfMoves >= 100) {
        return true;
    }
    // TODO: implement threefold repetition
    return false;
}
