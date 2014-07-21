// SEE.cpp
// created: 11:46PM 2/10/14
// author: Daniel Sparks
// license: See text/license.txt
// purpose: This cpp file handles the SEE type algorithms.

#include "exacto.h"
#include "inlines.h"

// ================================================================================
//  Static exchange evaluation root call (this is what assign_values calls, e.g.).
// ================================================================================

int16_t CGame::SEE(uint8_t source, uint8_t dest) {

    uint8_t attacker = board[source];
    int16_t value = piece_values[WHITE][board[dest]];
    bool was_occ = ((occupied & exp_2(dest)) != 0);

    SEE_make(wtm, source, dest);
    occupied |= exp_2(dest);
    value -= SEE_next(piece_values[WHITE][attacker], dest, btm);
    SEE_unmake(wtm, attacker, source, dest);

    if(!was_occ) occupied &= ~exp_2(dest);

    return value;
}

// ============================================
//  Static exchange evaluation, non-root call.
// ============================================

int16_t CGame::SEE_next(int16_t prev_piece, uint8_t square, bool color) {
    uint8_t     source = find_least_attacker(color, square);
    uint8_t     attacker = board[source];
    int16_t     value = 0;

    if(source != 255) {
        SEE_make(color, source, square);
        value = max(0, prev_piece - SEE_next(piece_values[WHITE][attacker], square, !color));
        SEE_unmake(color, attacker, source, square);
    }
    return value;
}

// ============================================================================
//  Make, unmake, and find least attacker are called only by SEE and SEE_next.
// ============================================================================

void CGame::SEE_make(bool color, uint8_t source, uint8_t dest) {
    pieces[color][board[source]] &= ~exp_2(source);
    occupied &= ~exp_2(source);
}

void CGame::SEE_unmake(bool color, uint8_t attacker, uint8_t source, uint8_t dest) {
    pieces[color][attacker] |= exp_2(source);
    occupied |= exp_2(source);
}

uint8_t CGame::find_least_attacker(bool color, uint8_t dest) {
    if(pawn_checks[color][dest] & pieces[color][PAWN])
        return bitscan(pawn_checks[color][dest] & pieces[color][PAWN]);
    if(knight_moves[dest] & pieces[color][KNIGHT])
        return bitscan(knight_moves[dest] & pieces[color][KNIGHT]);

    uint64_t x = bishop_moves[dest][((bishop_masks[dest] & occupied) * bishop_magics[dest]) >> BMS] & pieces[color][BISHOP];
    if(x) return bitscan(x);

    x = rook_moves[dest][((rook_masks[dest] & occupied) * rook_magics[dest]) >> RMS] & pieces[color][ROOK];
    if(x) return bitscan(x);

    x = (bishop_moves[dest][((bishop_masks[dest] & occupied) * bishop_magics[dest]) >> BMS]
            | rook_moves[dest][((rook_masks[dest] & occupied) * rook_magics[dest]) >> RMS]) & pieces[color][QUEEN];
    if(x) return bitscan(x);

    if(king_moves[dest] & pieces[color][KING])
        return bitscan(pieces[color][KING]);

    return 255;
}

// ===============================================================
//  This is a approximated version of static exchange evaluation.
// ===============================================================

int16_t CEngine::fuzzy_SEE(uint64_t SEE_bbs[2][7], int prev_capper, uint8_t square, bool color) {
    int16_t         value   = 0;
    uint8_t     i       = PAWN;

    for(; i <= KING; i++)
        if(exp_2(square) & SEE_bbs[color][i]) break;
    if(i <= 6) {
        SEE_bbs[color][i] &= ~exp_2(square);
        value = max(0, prev_capper - fuzzy_SEE(SEE_bbs, piece_values[WHITE][i], square, !color));
        SEE_bbs[color][i] |= exp_2(square);
    }
    return value;
}