#include "cexacto_evaluate.h"
#include "cexacto.h"
#include "cgame.h"

int CExacto::evaluate(CGame* game) {
    int score = 0;

    score += popcount(game->pieces[WHITE][PAWN]) * PAWN_VAL;
    score += popcount(game->pieces[WHITE][KNIGHT]) * KNIGHT_VAL;
    score += popcount(game->pieces[WHITE][BISHOP]) * BISHOP_VAL;
    score += popcount(game->pieces[WHITE][ROOK]) * ROOK_VAL;
    score += popcount(game->pieces[WHITE][QUEEN]) * QUEEN_VAL;

    score -= popcount(game->pieces[BLACK][PAWN]) * PAWN_VAL;
    score -= popcount(game->pieces[BLACK][KNIGHT]) * KNIGHT_VAL;
    score -= popcount(game->pieces[BLACK][BISHOP]) * BISHOP_VAL;
    score -= popcount(game->pieces[BLACK][ROOK]) * ROOK_VAL;
    score -= popcount(game->pieces[BLACK][QUEEN]) * QUEEN_VAL;

    return game->wtm? score : -score;
}