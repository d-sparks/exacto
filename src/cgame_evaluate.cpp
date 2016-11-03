#include "cgame_evaluate.h"
#include "cgame.h"

int CGame::evaluate() {
    int score = 0;

    score += popcount(pieces[WHITE][PAWN]) * PAWN_VAL;
    score += popcount(pieces[WHITE][KNIGHT]) * KNIGHT_VAL;
    score += popcount(pieces[WHITE][BISHOP]) * BISHOP_VAL;
    score += popcount(pieces[WHITE][ROOK]) * ROOK_VAL;
    score += popcount(pieces[WHITE][QUEEN]) * QUEEN_VAL;

    score -= popcount(pieces[BLACK][PAWN]) * PAWN_VAL;
    score -= popcount(pieces[BLACK][KNIGHT]) * KNIGHT_VAL;
    score -= popcount(pieces[BLACK][BISHOP]) * BISHOP_VAL;
    score -= popcount(pieces[BLACK][ROOK]) * ROOK_VAL;
    score -= popcount(pieces[BLACK][QUEEN]) * QUEEN_VAL;

    return wtm? score : -score;
}