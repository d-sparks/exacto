#pragma once
#include "SEE.h"
#include "cexacto_evaluate.cpp"
#include "bb.cpp"
#include "moves.cpp"
#include "squares.cpp"
#include "cgame.cpp"

namespace SEE {

    uint16_t pieceValues[7] = {
        0, PAWN_VAL, KNIGHT_VAL, BISHOP_VAL, ROOK_VAL, QUEEN_VAL, MATESCORE,
    };

    int16_t see(CGame* game, mv move) {
        BB sourceBB = exp_2(moves::source(move));
        ind dest = moves::dest(move);
        ind attacker = moves::attacker(move);
        int16_t score = pieceValues[moves::defender(move)];

        makeMove(game, sourceBB, attacker);
        score -= next(game, pieceValues[attacker], dest);
        unmakeMove(game, sourceBB, attacker);

        return score;
    }

    int16_t next(CBoard* board, int16_t previousVal, ind square) {
        ind attackerSquare = leastValuableAttackerSquare(board, square);
        if(attackerSquare > 63) {
            return 0;
        }
        ind attacker = board->board[attackerSquare] % 8;
        BB sourceBB = exp_2(attackerSquare);
        makeMove(board, sourceBB, attacker);
        int16_t value = max(0, previousVal - next(board, pieceValues[attacker], square));
        unmakeMove(board, sourceBB, attacker);
        return value;
    }

    void makeMove(CBoard* board, BB sourceBB, ind attacker) {
        board->pieces[board->wtm][attacker] &= ~sourceBB;
        board->occupied &= ~sourceBB;
        board->wtm = !board->wtm;
    }

    void unmakeMove(CBoard* board, BB sourceBB, ind attacker) {
        board->wtm = !board->wtm;
        board->pieces[board->wtm][attacker] |= sourceBB;
        board->occupied |= sourceBB;
    }

    ind leastValuableAttackerSquare(CBoard* board, ind square) {
        BB candidates = 0;
        do {
            bool wtm = board->wtm;
            // Pawns
            candidates = masks::PAWN_CHECKS[!wtm][square] & board->pieces[wtm][PAWN];
            if(candidates != 0) break;
            // Knights
            candidates = masks::KNIGHT_MOVES[square] & board->pieces[wtm][KNIGHT];
            if(candidates != 0) break;
            // Bishops
            BB bishopMoves = magics::bishopMoves(square, board->occupied);
            candidates = bishopMoves & board->pieces[wtm][BISHOP];
            if(candidates != 0) break;
            // Rooks
            BB rookMoves = magics::rookMoves(square, board->occupied);
            candidates = rookMoves & board->pieces[wtm][ROOK];
            if(candidates != 0) break;
            // Queens
            candidates = (bishopMoves | rookMoves) & board->pieces[wtm][QUEEN];
            if(candidates != 0) break;
            // King
            candidates = masks::KING_MOVES[square] & board->pieces[wtm][KING];
            if(candidates != 0) break;
            return 255;
        } while(false);
        return bitscan(candidates);
    }

};
