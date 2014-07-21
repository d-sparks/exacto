// movegen.cpp
// created: 1:20 PM 12/10/13
// author: Daniel Sparks
// license: See text/license.txt
// purpose: Functions pertaining to move generation.

#include "exacto.h"
#include "inlines.h"

uint64_t        rook_moves[64][4096],           // size unoptimized - sufficient for occupancy check
                bishop_moves[64][512],      // size unoptimized - sufficient for occupancy check

                pawn_checks[2][64];

/* Special flag key:
   1: Queen promotion
   2: Rook promotion
   3: Bishop promotion
   4: Knight promotion
   5: White double pawn move
   6: Black double pawn move
   7: White en passant capture
   8: Black en passant capture
   9: White kingside castling
   10: White queenside castling
   11: Black kingside castling
   12: Black queenside castling */

// =============================================================
//  Generates capture moves and checking moves in any position.
// =============================================================

void CGame::generate_caps(uint16_t * movelist) {
    uint64_t SEE_bbs[2][7] = {0};
    attack_set_gen(btm, SEE_bbs);
    cap_gen(movelist, SEE_bbs);
}

void CGame::cap_gen(uint16_t * movelist, uint64_t SEE_bbs[2][7]) {

    uint64_t        x,
                    enemy_bishop_attacks    = SEE_bbs[btm][BISHOP] | SEE_bbs[btm][QUEEN],
                    enemy_rook_attacks      = SEE_bbs[btm][ROOK] | SEE_bbs[btm][QUEEN],
                    not_pinned_by_bishop    = all_set,
                    not_pinned_by_rook      = all_set,
                    not_pinned_pieces;
    uint8_t         i,
                    king                    = bitscan(pieces[wtm][KING]);

    /* First, find any pinned pieces by finding which bishop/rook moves from our king hit pieces which are attacked by enemy bishops/rooks. */

    x = (enemy_bishop_attacks & pieces[wtm][ALL]) & bishop_moves[king][((bishop_masks[king] & occupied) * bishop_magics[king]) >> BMS]; // Bishops first
    while(x) {
        i = bitscan(x);
        if((opposite_masks[king][i] & bishop_moves[i][((bishop_masks[i] & occupied) * bishop_magics[i]) >> BMS]) & (pieces[btm][BISHOP] | pieces[btm][QUEEN]))
            not_pinned_by_bishop &= ~exp_2(i);
        x &= x - 1;
    }
    x = (enemy_rook_attacks & pieces[wtm][ALL]) & rook_moves[king][((rook_masks[king] & occupied) * rook_magics[king]) >> RMS]; // Rooks next
    while(x) {
        i = bitscan(x);
        if((opposite_masks[king][i] & rook_moves[i][((rook_masks[i] & occupied) * rook_magics[i]) >> RMS]) & (pieces[btm][ROOK] | pieces[btm][QUEEN]))
            not_pinned_by_rook &= ~exp_2(i);
        x &= x - 1;
    }

    not_pinned_pieces = not_pinned_by_bishop & not_pinned_by_rook;

    uint64_t        checks_bishop,
                    checks_rook;
    uint8_t         enemy_king = bitscan(pieces[btm][KING]);

    checks_bishop = (bishop_moves[enemy_king][((bishop_masks[enemy_king] & occupied) * bishop_magics[enemy_king]) >> BMS] & empty);
    checks_rook = (rook_moves[enemy_king][((rook_masks[enemy_king] & occupied) * rook_magics[enemy_king]) >> RMS] & empty);

    /* Now generate captures. */

    if(wtm){
        x = ((pieces[WHITE][PAWN] & not_pinned_pieces) << 8) & empty;
        serp_w(x & ((uint64_t)0xff00000000000000 | pawn_checks[WHITE][enemy_king]), &movelist);                                                 /* Promoting / checking moves */
        serp_w_d((((x & (uint64_t)0x0000000000ff0000) << 8) & (pawn_checks[WHITE][enemy_king] & empty)), &movelist);                            /* Double pawn moves giving check */
        serp_w_capleft(((((pieces[WHITE][PAWN] & not_pinned_pieces) & ((uint64_t)0x7f7f7f7f7f7f7f7f)) << 9) & pieces[BLACK][ALL]), &movelist);  /* Pawn caps left */
        serp_w_capright(((((pieces[WHITE][PAWN] & not_pinned_pieces) & ((uint64_t)0xfefefefefefefefe)) << 7) & pieces[BLACK][ALL]), &movelist); /* Pawn caps right */
    } else {
        x = ((pieces[BLACK][PAWN] & not_pinned_pieces) >> 8) & empty;
        serp_b(x & ((uint64_t)0x00000000000000ff | pawn_checks[BLACK][enemy_king]), &movelist);                                                 /* Promoting / checking moves */
        serp_b_d((((x & (uint64_t)0x0000ff0000000000) >> 8) & (pawn_checks[BLACK][enemy_king] & empty)), &movelist);                            /* Double pawn moves giving check */
        serp_b_capleft(((((pieces[BLACK][PAWN] & not_pinned_pieces) & ((uint64_t)0x7f7f7f7f7f7f7f7f)) >> 7) & pieces[WHITE][ALL]), &movelist);  /* Pawn caps left */
        serp_b_capright(((((pieces[BLACK][PAWN] & not_pinned_pieces) & ((uint64_t)0xfefefefefefefefe)) >> 9) & pieces[WHITE][ALL]), &movelist); /* Pawn caps right */
    }

    /* En passant is covered separately */

    if(en_passant) {
        x = pieces[wtm][PAWN] & pawn_checks[wtm][bitscan(en_passant)] & not_pinned_pieces;

        do {
            if(x == 0) break;
            if(((pieces[wtm][KING] & mask_ep_ranks[wtm]) != 0)
                    && (((pieces[btm][ROOK] | pieces[btm][QUEEN]) & mask_ep_ranks[wtm]) != 0)
                    && (pop_count(x) == 1)) {

                /*  We take care of pins like this now.  Presumably this code here will be executed fairly rarely:

                    +---+---+---+---+---+---+---+---+
                    |   |   |   |   | : |   |   |   |
                    +---+---+---+---+---+---+---+---+
                    |   | r |   | P | p |   | K |   |
                    +---+---+---+---+---+---+---+---+       */

                uint64_t y = (pieces[btm][ROOK] | pieces[btm][QUEEN]) & mask_ep_ranks[wtm];
                while(y) {
                    i = bitscan(y);
                    /*  We know at this point that there is exactly one pawn threaning the en passant square.  If that pawn lies
                        between the enemy rook/queen and the friendly king, then so does the doubly-moved pawn.  Therefore, if there are
                        only two occupied squares between the rook/queen and king, we are in the above situation. */
                    if(((interceding_squares[i][king] & x) != 0) && (pop_count(interceding_squares[i][king] & occupied) == 2)) {
                        x = 0;
                        break;
                    }
                    y &= ~exp_2(i);
                }
                if(x == 0) break;
            }
            serp_ep(x, &movelist, bitscan(en_passant));
        } while (false);
    }

    x = pieces[wtm][KNIGHT] & not_pinned_pieces;                        /* Knights */
    while(x){
        i = bitscan(x);
        serialize_piece(knight_moves[i] & (pieces[btm][0] | (knight_moves[enemy_king] & empty)), &movelist, i);
        x &= (~exp_2(i));
    }

    x = pieces[wtm][BISHOP] & not_pinned_pieces;                        /* Bishops */
    while(x){
        i = bitscan(x);
        serialize_piece((bishop_moves[i][((bishop_masks[i] & occupied) * bishop_magics[i]) >> BMS]) & (pieces[btm][ALL] | checks_bishop), &movelist, i);
        x &= (~exp_2(i));
    }

    x = pieces[wtm][ROOK] & not_pinned_pieces;                          /* Rooks */
    while(x){
        i = bitscan(x);
        serialize_piece((rook_moves[i][((rook_masks[i] & occupied) * rook_magics[i]) >> RMS]) & (pieces[btm][ALL] | checks_rook), &movelist, i);
        x &= (~exp_2(i));
    }

    x = pieces[wtm][QUEEN] & not_pinned_pieces;                         /* Queens */
    while(x) {
        i = bitscan(x);
        serialize_piece((rook_moves[i][((rook_masks[i] & occupied) * rook_magics[i]) >> RMS]) & (pieces[btm][ALL] | checks_rook), &movelist, i);
        serialize_piece((bishop_moves[i][((bishop_masks[i] & occupied) * bishop_magics[i]) >> BMS]) & (pieces[btm][ALL]  | checks_bishop ), &movelist, i);
        x &= (~exp_2(i));
    }

    /*  Pinned piece moves are generated here.  */

    if(~not_pinned_pieces) {
        // Pinned pawns
        x = pieces[wtm][PAWN] & (~not_pinned_pieces);
        while(x) {
            i = bitscan(x);
            if(wtm) {
                if((i % 8) > 0) serp_w_capright(((exp_2(i) << 7) & (pieces[BLACK][ALL] | en_passant)) & opposite_masks[king][i], &movelist);
                if((i % 8) < 7) serp_w_capleft(((exp_2(i) << 9) & (pieces[BLACK][ALL] | en_passant)) & opposite_masks[king][i], &movelist);
            } else {
                if((i % 8) < 7) serp_b_capleft(((exp_2(i) >> 7) & (pieces[WHITE][ALL] | en_passant)) & opposite_masks[king][i], &movelist);
                if((i % 8) > 0) serp_b_capright(((exp_2(i) >> 9) & (pieces[WHITE][ALL] | en_passant)) & opposite_masks[king][i], &movelist);
            }
            x &= x - 1;
        }
        // Pinned bishops/queens
        x = (pieces[wtm][BISHOP] | pieces[wtm][QUEEN]) & (~not_pinned_pieces);
        while(x) {
            i = bitscan(x);
            serialize_piece(bishop_moves[i][((bishop_masks[i] & occupied) * bishop_magics[i]) >> BMS] & opposite_masks[king][i] & pieces[btm][ALL], &movelist, i);
            x &= x - 1;
        }
        // Pinned rooks/queens
        x = (pieces[wtm][ROOK] | pieces[wtm][QUEEN]) & (~not_pinned_pieces);
        while(x) {
            i = bitscan(x);
            serialize_piece(rook_moves[i][((rook_masks[i] & occupied) * rook_magics[i]) >> RMS] & opposite_masks[king][i] & pieces[btm][ALL], &movelist, i);
            x &= x - 1;
        }
    }

    x = king_moves[king] & (~SEE_bbs[btm][ALL]) & pieces[btm][ALL];     /* King */
    serialize_piece(x, &movelist, king);

    /* Sometimes castling can put the enemy king in check, but we don't generate those moves. */

    *movelist = 0;
}

// ========================================================================================
//  Generates all legal moves in any position (requires pseudo_gen and pins to be run first).
// ========================================================================================

void CGame::generate_moves(uint16_t * movelist) {
    uint64_t SEE_bbs[2][7] = {0};
    move_gen(movelist, SEE_bbs);
}

void CGame::move_gen(uint16_t * movelist, uint64_t SEE_bbs[2][7]) {

/*  For purposes of legal move generation and move ordering, we find "pseudo-legal" moves of our opponent first.
    This is also used during the static exchange evaluation.    */

    attack_set_gen(btm, SEE_bbs);

    uint64_t        x,
                    enemy_bishop_attacks    = SEE_bbs[btm][BISHOP] | SEE_bbs[btm][QUEEN],
                    enemy_rook_attacks      = SEE_bbs[btm][ROOK] | SEE_bbs[btm][QUEEN],
                    not_pinned_by_bishop    = all_set,
                    not_pinned_by_rook      = all_set,
                    not_pinned_pieces;
    uint8_t         i,
                    king                    = bitscan(pieces[wtm][KING]);





    // Now find any pinned pieces by finding which bishop/rook moves from our king hit pieces which are attacked by enemy bishops/rooks.

    x = (enemy_bishop_attacks & pieces[wtm][0]) & bishop_moves[king][((bishop_masks[king] & occupied) * bishop_magics[king]) >> BMS];
    while(x) {
        i = bitscan(x);
        if((opposite_masks[king][i] & bishop_moves[i][((bishop_masks[i] & occupied) * bishop_magics[i]) >> BMS]) & (pieces[btm][BISHOP] | pieces[btm][QUEEN]))
            not_pinned_by_bishop &= ~exp_2(i);
        x &= x - 1;
    }
    x = (enemy_rook_attacks & pieces[wtm][0]) & rook_moves[king][((rook_masks[king] & occupied) * rook_magics[king]) >> RMS];
    while(x) {
        i = bitscan(x);
        if((opposite_masks[king][i] & rook_moves[i][((rook_masks[i] & occupied) * rook_magics[i]) >> RMS]) & (pieces[btm][ROOK] | pieces[btm][QUEEN]))
            not_pinned_by_rook &= ~exp_2(i);
        x &= x - 1;
    }

    not_pinned_pieces = not_pinned_by_bishop & not_pinned_by_rook;

    // Cases split based on whether we are in check.

    if(SEE_bbs[btm][ALL] & pieces[wtm][KING]) {

        // We are in check, so we find which moves (if any) can get us out of check.

        uint64_t    pawn_attackers      = pawn_checks[btm][king] & pieces[btm][1],
                    knight_attackers    = knight_moves[king] & pieces[btm][2],
                    bishop_attackers,
                    rook_attackers,
                    all_attackers,
                    intercept;

        // If threatened by bishops/queens, find their squares

        if(enemy_bishop_attacks & exp_2(king))
            bishop_attackers = bishop_moves[king][((bishop_masks[king] & occupied) * bishop_magics[king]) >> BMS] & (pieces[btm][3] | pieces[btm][5]);
        else
            bishop_attackers = 0;

        // If threatened by rooks/queens, find their squares

        if(enemy_rook_attacks & exp_2(king))
            rook_attackers = rook_moves[king][((rook_masks[king] & occupied) * rook_magics[king]) >> RMS] & (pieces[btm][4] | pieces[btm][5]);
        else
            rook_attackers = 0;

        all_attackers = pawn_attackers | (knight_attackers | (bishop_attackers | (rook_attackers)));

        if(pop_count(all_attackers) == 1) {

            // Only one checking piece, which means we can try to kill it or intercept it to get out of check

            if(bishop_attackers | rook_attackers)
                intercept = interceding_squares[king][bitscan(all_attackers)];
            else
                intercept = 0;

            // Find intercepting moves to square i.  Note that pinned pieces can never make these moves.  Rooks can't lose castling making these moves.

            while(intercept) {
                i = bitscan(intercept);

                // Pawns

                if(wtm) {
                    if(i > 15) {
                        serp_from_dest(exp_2(i-8) & pieces[1][1] & not_pinned_pieces, &movelist, i);
                        if(i < 32)
                            if(empty & exp_2(i-8))
                                serp_d_from_dest(exp_2(i-16) & (pieces[1][1] & not_pinned_pieces), &movelist, i);
                    }
                } else {
                    if(i < 48) {
                        serp_from_dest(exp_2(i+8) & (pieces[0][1] & not_pinned_pieces), &movelist, i);
                        if(i > 31)
                            if(empty & exp_2(i+8))
                                serp_d_from_dest(exp_2(i+16) & (pieces[0][1] & not_pinned_pieces), &movelist, i);
                    }
                }

                // Knights, bishops, rooks
                serialize_from_dest(knight_moves[i] & (pieces[wtm][2] & not_pinned_pieces), &movelist, i);
                serialize_from_dest(bishop_moves[i][((bishop_masks[i] & occupied) * bishop_magics[i]) >> BMS] &
                            ((pieces[wtm][3] | pieces[wtm][5]) & not_pinned_pieces), &movelist, i);
                serialize_from_dest(rook_moves[i][((rook_masks[i] & occupied) * rook_magics[i]) >> RMS] &
                            ((pieces[wtm][4] | pieces[wtm][5]) & not_pinned_pieces), &movelist, i);

                intercept &= ~exp_2(i);
            }

            // Find all ways to kill the attacker, via pawns, knights, bishops and rooks.

            i = bitscan(all_attackers);
            serp_from_dest(pawn_checks[wtm][i] & (pieces[wtm][1] & not_pinned_pieces), &movelist, i);
            if((en_passant != 0) && (board[i] == 1))
                serp_ep_from_dest(pawn_checks[wtm][bitscan(en_passant)] & (pieces[wtm][PAWN] & not_pinned_pieces), &movelist, bitscan(en_passant));
            serialize_from_dest(knight_moves[i] & (pieces[wtm][2] & not_pinned_pieces), &movelist, i);
            serialize_from_dest(bishop_moves[i][((bishop_masks[i] & occupied) * bishop_magics[i]) >> BMS] &
                        ((pieces[wtm][3] | pieces[wtm][5]) & not_pinned_pieces), &movelist, i);
            serialize_from_dest(rook_moves[i][((rook_masks[i] & occupied) * rook_magics[i]) >> RMS] &
                        ((pieces[wtm][4] | pieces[wtm][5]) & not_pinned_pieces), &movelist, i);
        }

        // Now find simple moves of the king to evade check
        x = king_moves[king] & (~(SEE_bbs[btm][0] | pieces[wtm][0]));
        serialize_piece(x, &movelist, king);

    } else {

        // We are not in check, so we generate regular moves

        // Pawns
        if(wtm){
            x = ((pieces[WHITE][PAWN] & not_pinned_pieces) << 8) & empty;
            serp_w(x, &movelist);                                                                                                       /* Regular pawn moves */
            serp_w_d((((x & mask_rank[2]) << 8) & empty), &movelist);                                                                   /* Double pawn moves */
            serp_w_capleft((((pieces[WHITE][PAWN] & not_pinned_pieces) & not_mask_file[0]) << 9) & pieces[BLACK][ALL], &movelist);      /* Captures to the left */
            serp_w_capright((((pieces[WHITE][PAWN] & not_pinned_pieces) & not_mask_file[7]) << 7) & pieces[BLACK][ALL], &movelist);     /* Captures to the right */
        } else {
            x = ((pieces[BLACK][PAWN] & not_pinned_pieces) >> 8) & empty;
            serp_b(x, &movelist);                                                                                                       /* Regular pawn moves */
            serp_b_d((((x & mask_rank[5]) >> 8) & empty), &movelist);                                                                   /* Double pawn moves */
            serp_b_capleft((((pieces[BLACK][PAWN] & not_pinned_pieces) & not_mask_file[0]) >> 7) & pieces[WHITE][ALL], &movelist);      /* Captures to the left */
            serp_b_capright((((pieces[BLACK][PAWN] & not_pinned_pieces) & not_mask_file[7]) >> 9) & pieces[WHITE][ALL], &movelist);     /* Captures to the right */
        }

        // En passant is covered separately
        if(en_passant) {
            x = pieces[wtm][PAWN] & pawn_checks[wtm][bitscan(en_passant)] & not_pinned_pieces;

            do {
                if(x == 0) break;
                if(((pieces[wtm][KING] & mask_ep_ranks[wtm]) != 0)
                        && (((pieces[btm][ROOK] | pieces[btm][QUEEN]) & mask_ep_ranks[wtm]) != 0)
                        && (pop_count(x) == 1)) {

                    /*  We take care of pins like this now.  Presumably this code here will be executed fairly rarely:

                        +---+---+---+---+---+---+---+---+
                        |   |   |   |   | : |   |   |   |
                        +---+---+---+---+---+---+---+---+
                        |   | r |   | P | p |   | K |   |
                        +---+---+---+---+---+---+---+---+       */

                    uint64_t y = (pieces[btm][ROOK] | pieces[btm][QUEEN]) & mask_ep_ranks[wtm];
                    while(y) {
                        i = bitscan(y);
                        /*  We know at this point that there is exactly one pawn threaning the en passant square.  If that pawn lies
                            between the enemy rook/queen and the friendly king, then so does the doubly-moved pawn.  Therefore, if there are
                            only two occupied squares between the rook/queen and king, we are in the above situation. */
                        if(((interceding_squares[i][king] & x) != 0) && (pop_count(interceding_squares[i][king] & occupied) == 2)) {
                            x = 0;
                            break;
                        }
                        y &= ~exp_2(i);
                    }
                    if(x == 0) break;
                }
                serp_ep(x, &movelist, bitscan(en_passant));
            } while (false);
        }

        // Knights
        x = pieces[wtm][2] & not_pinned_pieces;
        while(x){
            i = bitscan(x);
            serialize_piece(knight_moves[i] & (~pieces[wtm][ALL]), &movelist, i);
            x &= (~exp_2(i));
        }

        // Bishops
        x = pieces[wtm][3] & not_pinned_pieces;
        while(x){
            i = bitscan(x);
            serialize_piece((bishop_moves[i][((bishop_masks[i] & occupied) * bishop_magics[i]) >> BMS]) & (~pieces[wtm][ALL]), &movelist, i);
            x &= (~exp_2(i));
        }

        // Rooks
        x = pieces[wtm][4] & not_pinned_pieces;
        while(x){
            i = bitscan(x);
            serialize_piece((rook_moves[i][((rook_masks[i] & occupied) * rook_magics[i]) >> RMS]) & (~pieces[wtm][ALL]), &movelist, i);
            x &= (~exp_2(i));
        }

        // Queens
        x = pieces[wtm][5] & not_pinned_pieces;
        while(x){
            i = bitscan(x);
            serialize_piece((rook_moves[i][((rook_masks[i] & occupied) * rook_magics[i]) >> RMS]) & (~pieces[wtm][ALL]), &movelist, i);
            serialize_piece((bishop_moves[i][((bishop_masks[i] & occupied) * bishop_magics[i]) >> BMS]) & (~pieces[wtm][ALL]), &movelist, i);
            x &= (~exp_2(i));
        }

        // King
        x = king_moves[king];
        x &= (~(SEE_bbs[btm][0] | pieces[wtm][0]));
        serialize_piece(x, &movelist, king);
        x &= empty;
        x |= x >> 1;
        if(empty & (exp_2(king) << 3)) x |= x << 1;
        serialize_castling((x & castling[wtm]) & (~(SEE_bbs[btm][0] | occupied)), &movelist);       // Castling

        // Pinned pieces can sometimes move, so we generate those moves. (Note we don't attempt to generate these if we are in check.)

        if(~not_pinned_pieces) {
            // Pinned pawns
            x = pieces[wtm][PAWN] & (~not_pinned_pieces);
            while(x) {
                i = bitscan(x);
                if(wtm) {
                    uint64_t z = exp_2(i) << 8;
                    if((i % 8) == (king %8)) {              // I believe this can be optimized: don't declare z, but use if statements on i
                        serp_w(z & empty, &movelist);       // And inside the function calls do exp_2(i) << 9 etc.    Another general idea: have serp_w take pointer
                        if((z & empty) & mask_rank[2])      // and unset the bits of pawns that didn't move, and pass that directly into serp_*_d.
                            serp_w_d((z << 8) & empty, &movelist);
                    }
                    serp_w_capleft((((z & not_mask_file[0]) << 1) & (pieces[BLACK][ALL] | en_passant)) & opposite_masks[king][i], &movelist);
                    serp_w_capright((((z & not_mask_file[7]) >> 1) & (pieces[BLACK][ALL] | en_passant)) & opposite_masks[king][i], &movelist);
                } else {
                    uint64_t z = exp_2(i) >> 8;
                    if((i % 8) == (king % 8)) {
                        serp_b(z & empty, &movelist);
                        if((z & empty) & mask_rank[5])
                            serp_b_d((z >> 8) & empty, &movelist);
                    }
                    serp_b_capleft((((z & not_mask_file[0]) << 1) & (pieces[WHITE][ALL] | en_passant)) & opposite_masks[king][i], &movelist);
                    serp_b_capright((((z & not_mask_file[7]) >> 1) & (pieces[WHITE][ALL] | en_passant)) & opposite_masks[king][i], &movelist);
                }
                x &= ~exp_2(i);
            }
            // Pinned bishops/queens
            x = (pieces[wtm][3] | pieces[wtm][5]) & (~not_pinned_pieces);
            while(x) {
                i = bitscan(x);
                serialize_piece(bishop_moves[i][((bishop_masks[i] & occupied) * bishop_magics[i]) >> BMS] & (opposite_masks[king][i] | interceding_squares[king][i]), &movelist, i);
                x &= ~exp_2(i);
            }
            // Pinned rooks/queens
            x = (pieces[wtm][4] | pieces[wtm][5]) & (~not_pinned_pieces);
            while(x) {
                i = bitscan(x);
                serialize_piece(rook_moves[i][((rook_masks[i] & occupied) * rook_magics[i]) >> RMS] & (opposite_masks[king][i] | interceding_squares[king][i]), &movelist, i);
                x &= ~exp_2(i);
            }
        }

    }

    *movelist = 0;

}


// =======================================
//  Returns true if the king is in check.
// =======================================

bool CGame::in_check() {
    uint8_t i = bitscan(pieces[wtm][6]);
    if(bishop_moves[i][((bishop_masks[i] & occupied) * bishop_magics[i]) >> BMS] & (pieces[btm][3] | pieces[btm][5])) return true;
    if(rook_moves[i][((rook_masks[i] & occupied) * rook_magics[i]) >> RMS] & (pieces[btm][4] | pieces[btm][5])) return true;
    if(knight_moves[i] & pieces[btm][2]) return true;
    if(pawn_checks[btm][i] & pieces[btm][1]) return true;
    return false;
}


/* ==================================================================================
    SEE_gen fills an array with bitboards which indicate which types of piece can
    attack which squares.  This is used to approximate a static exchange evaluation.
        - Note, some of this code could be mixed into move_gen?
   ================================================================================== */

void CGame::attack_set_gen(bool color, uint64_t SEE_bbs[2][7]) {
    uint64_t        y;
    uint8_t i;

/*  Pawns.  */

    if(color)   SEE_bbs[WHITE][PAWN] = (((pieces[WHITE][PAWN] & not_mask_file[0]) << 9) | ((pieces[WHITE][PAWN] & not_mask_file[7]) << 7));
    else        SEE_bbs[BLACK][PAWN] = (((pieces[BLACK][PAWN] & not_mask_file[0]) >> 7) | ((pieces[BLACK][PAWN] & not_mask_file[7]) >> 9));

/*  Knights.    */

    y = pieces[color][KNIGHT];
    while(y) {
        i = bitscan(y);
        SEE_bbs[color][KNIGHT] |= knight_moves[i];
        y &= ~exp_2(i);
    }

/*  Bishops.    */

    y = pieces[color][BISHOP];
    while(y) {
        i = bitscan(y);
        SEE_bbs[color][BISHOP] |= bishop_moves[i][((bishop_masks[i] & (occupied & (~pieces[!color][KING]))) * bishop_magics[i]) >> BMS];
        y &= ~exp_2(i);
    }

/*  Rooks.  */

    y = pieces[color][ROOK];
    while(y) {
        i = bitscan(y);
        SEE_bbs[color][ROOK] |= rook_moves[i][((rook_masks[i] & (occupied & (~pieces[!color][KING]))) * rook_magics[i]) >> RMS];
        y &= ~exp_2(i);
    }

/*  Queens. */

    y = pieces[color][QUEEN];
    while(y) {
        i = bitscan(y);
        SEE_bbs[color][QUEEN] |= bishop_moves[i][((bishop_masks[i] & (occupied & (~pieces[!color][KING]))) * bishop_magics[i]) >> BMS];
        SEE_bbs[color][QUEEN] |= rook_moves[i][((rook_masks[i] & (occupied & (~pieces[!color][KING]))) * rook_magics[i]) >> RMS];
        y &= ~exp_2(i);
    }

/*  Kings.  */

    SEE_bbs[color][KING] = king_moves[bitscan(pieces[color][KING])];

/* "All" = Pawn, knight, bishop, rook and queen pseudo attacks. */

    SEE_bbs[color][ALL] = SEE_bbs[color][PAWN] | SEE_bbs[color][KNIGHT] | SEE_bbs[color][BISHOP] | SEE_bbs[color][ROOK] | SEE_bbs[color][QUEEN] | SEE_bbs[color][KING];

}


void CGame::attack_set_gen_light(bool color, uint64_t * bb) {

    /*  Pawns.  */

    if(color)   *bb = (((pieces[WHITE][PAWN] & not_mask_file[0]) << 9) | ((pieces[WHITE][PAWN] & not_mask_file[7]) << 7));
    else        *bb = (((pieces[BLACK][PAWN] & not_mask_file[0]) >> 7) | ((pieces[BLACK][PAWN] & not_mask_file[7]) >> 9));

    /*  Knights.    */

    uint8_t i;
    uint64_t y = pieces[color][KNIGHT];
    while(y) {
        i = bitscan(y);
        *bb |= knight_moves[i];
        y &= y - 1;
    }

    /*  Bishops.    */

    y = pieces[color][BISHOP];
    while(y) {
        i = bitscan(y);
        *bb |= bishop_moves[i][((bishop_masks[i] & (occupied & (~pieces[!color][KING]))) * bishop_magics[i]) >> BMS];
        y &= y - 1;
    }

    /*  Rooks.  */

    y = pieces[color][ROOK];
    while(y) {
        i = bitscan(y);
        *bb |= rook_moves[i][((rook_masks[i] & (occupied & (~pieces[!color][KING]))) * rook_magics[i]) >> RMS];
        y &= y - 1;
    }

    /*  Queens. */

    y = pieces[color][QUEEN];
    while(y) {
        i = bitscan(y);
        *bb |= bishop_moves[i][((bishop_masks[i] & (occupied & (~pieces[!color][KING]))) * bishop_magics[i]) >> BMS];
        *bb |= rook_moves[i][((rook_masks[i] & (occupied & (~pieces[!color][KING]))) * rook_magics[i]) >> RMS];
        y &= y - 1;
    }

    /*  Kings.  */

    *bb |= king_moves[bitscan(pieces[color][KING])];

}



// =============================
//  serp = Serialize pawn moves
// =============================


void CGame::serp_w(uint64_t bb, uint16_t ** movelist) {
    char i;
    uint64_t x = bb & not_mask_rank[7];

    // Non-promoting moves
    while(x) {
        i = bitscan(x);
        *((*movelist)++) = encode_move(i-8, i, 0);
        x &= ~exp_2(i);
    }

    // Promoting moves
    x = bb & mask_rank[7];
    while(x) {
        i = bitscan(x);
        *((*movelist)++) = encode_move(i-8, i, 1);          // Queen promotion
        *((*movelist)++) = encode_move(i-8, i, 2);          // Rook promotion
        *((*movelist)++) = encode_move(i-8, i, 3);          // Bishop promotion
        *((*movelist)++) = encode_move(i-8, i, 4);          // Knight promotion
        x &= ~exp_2(i);
    }
}

void CGame::serp_b(uint64_t bb, uint16_t ** movelist) {
    char i;
    uint64_t x = bb & not_mask_rank[0];

    // Non-promoting moves
    while(x) {
        i = bitscan(x);
        *((*movelist)++) = encode_move(i+8, i, 0);
        x &= ~exp_2(i);
    }

    // Promoting moves
    x = bb & mask_rank[0];
    while(x) {
        i = bitscan(x);
        *((*movelist)++) = encode_move(i+8, i, 1);          // Queen promotion
        *((*movelist)++) = encode_move(i+8, i, 2);          // Rook promotion
        *((*movelist)++) = encode_move(i+8, i, 3);          // Bishop promotion
        *((*movelist)++) = encode_move(i+8, i, 4);          // Knight promotion
        x &= ~exp_2(i);
    }
}

void CGame::serp_w_d(uint64_t bb, uint16_t ** movelist) {
    char i;
    uint64_t x = bb;

    while(x) {
        i = bitscan(x);
        *((*movelist)++) = encode_move(i-16, i, 5);
        x &= ~exp_2(i);
    }
}

void CGame::serp_b_d(uint64_t bb, uint16_t ** movelist) {
    char i;
    uint64_t x = bb;

    while(x) {
        i = bitscan(x);
        *((*movelist)++) = encode_move(i+16, i, 6);
        x &= ~exp_2(i);
    }
}

void CGame::serp_w_capleft(uint64_t bb, uint16_t ** movelist) {
    char i;
    uint64_t x = bb & not_mask_rank[7];

    // Non promoting captures
    while(x) {
        i = bitscan(x);
        if(en_passant & exp_2(i))
            *((*movelist)++) = encode_move(i-9, i, 7);      // En passant capture
        else
            *((*movelist)++) = encode_move(i-9, i, 0);      // Regular capture
        x &= ~exp_2(i);
    }

    // Promoting captures
    x = bb & mask_rank[7];
    while(x){
        i = bitscan(x);
        *((*movelist)++) = encode_move(i-9, i, 1);          // Queen promotion
        *((*movelist)++) = encode_move(i-9, i, 2);          // Rook promotion
        *((*movelist)++) = encode_move(i-9, i, 3);          // Bishop promotion
        *((*movelist)++) = encode_move(i-9, i, 4);          // Knight promotion
        x &= ~exp_2(i);
    }
}

void CGame::serp_w_capright(uint64_t bb, uint16_t ** movelist) {
    char i;
    uint64_t x = bb & not_mask_rank[7];

    // Non promoting captures
    while(x) {
        i = bitscan(x);
        if(en_passant & exp_2(i))
            *((*movelist)++) = encode_move(i-7, i, 7);      // En passant capture
         else
            *((*movelist)++) = encode_move(i-7, i, 0);      // Regular pawn capture
        x &= ~exp_2(i);
    }

    // Promoting captures
    x = bb & mask_rank[7];
    while(x){
        i = bitscan(x);
        *((*movelist)++) = encode_move(i-7, i, 1);          // Queen promotion
        *((*movelist)++) = encode_move(i-7, i, 2);          // Rook promotion
        *((*movelist)++) = encode_move(i-7, i, 3);          // Bishop promotion
        *((*movelist)++) = encode_move(i-7, i, 4);          // Knight promotion
        x &= ~exp_2(i);
    }
}

void CGame::serp_b_capleft(uint64_t bb, uint16_t ** movelist) {
    char i;
    uint64_t x = bb & not_mask_rank[0];

    // Non promoting captures
    while(x) {
        i = bitscan(x);
        if(en_passant & exp_2(i))
            *((*movelist)++) = encode_move(i+7, i, 8);      // En passant capture
         else
            *((*movelist)++) = encode_move(i+7, i, 0);      // Regular capture
        x &= ~exp_2(i);
    }

    // Promoting captures
    x = bb & mask_rank[0];
    while(x){
        i = bitscan(x);
        *((*movelist)++) = encode_move(i+7, i, 1);          // Queen promotion
        *((*movelist)++) = encode_move(i+7, i, 2);          // Rook promotion
        *((*movelist)++) = encode_move(i+7, i, 3);          // Bishop promotion
        *((*movelist)++) = encode_move(i+7, i, 4);          // Knight promotion
        x &= ~exp_2(i);
    }
}

void CGame::serp_b_capright(uint64_t bb, uint16_t **movelist) {
    char i;
    uint64_t x = bb & not_mask_rank[0];

    // Non promoting captures
    while(x) {
        i = bitscan(x);
        if(en_passant & exp_2(i))
            *((*movelist)++) = encode_move(i+9, i, 8);      // En passant capture
        else
            *((*movelist)++) = encode_move(i+9, i, 0);      // Regular capture
        x &= ~exp_2(i);
    }

    // Promoting captures
    x = bb & mask_rank[0];
    while(x){
        i = bitscan(x);
        *((*movelist)++) = encode_move(i+9, i, 1);          // Queen promotion
        *((*movelist)++) = encode_move(i+9, i, 2);          // Rook promotion
        *((*movelist)++) = encode_move(i+9, i, 3);          // Bishop promotion
        *((*movelist)++) = encode_move(i+9, i, 4);          // Knight promotion
        x &= ~exp_2(i);
    }
}

void CGame::serp_ep(uint64_t bb, uint16_t ** movelist, uint8_t ep_square) {
    uint8_t i = bitscan(bb);
    *((*movelist)++) = encode_move(i, ep_square, wtm? 7 : 8);
    if(bb & ~exp_2(i)) {
        i = bitscan(bb & ~exp_2(i));
        *((*movelist)++) = encode_move(i, ep_square, wtm? 7 : 8);
    }
}


// ===============================================
//  Other serialize functions for move generation
// ===============================================

void CGame::serialize_piece(uint64_t bb, uint16_t ** movelist, uint8_t source_square) {
    int i;
    while(bb) {
        i = bitscan(bb);
        *((*movelist)++) = encode_move(source_square, i, 0);
        bb &= ~exp_2(i);
    }
}

void CGame::serialize_castling(uint64_t bb, uint16_t ** movelist) {
    if(wtm) {
        if(bb & exp_2(1))
            *((*movelist)++) = encode_move(3, 1, 9);
        if(bb & exp_2(5))
            *((*movelist)++) = encode_move(3, 5, 10);
    } else {
        if(bb & exp_2(57))
            *((*movelist)++) = encode_move(59, 57, 11);
        if(bb & exp_2(61))
            *((*movelist)++) = encode_move(59, 61, 12);
    }
}


// =============================================================================================
//  Serialize from destination, for things like intercepting check or killing a check-attacker.
// =============================================================================================

void CGame::serialize_from_dest(uint64_t bb, uint16_t ** movelist, uint8_t dest_square) {
    int i;
    while(bb) {
        i = bitscan(bb);
        *((*movelist)++) = encode_move(i, dest_square, 0);
        bb &= ~exp_2(i);
    }
}

void CGame::serp_d_from_dest(uint64_t bb, uint16_t ** movelist, uint8_t dest_square) {
    int i;
    while(bb) {
        i = bitscan(bb);
        *((*movelist)++) = encode_move(i, dest_square, 6 - wtm);
        bb &= ~exp_2(i);
    }
}

void CGame::serp_ep_from_dest(uint64_t bb, uint16_t ** movelist, uint8_t dest_square) {
    int i;
    while(bb) {
        i = bitscan(bb);
        *((*movelist)++) = encode_move(i, dest_square, 8 - wtm);
        bb &= ~exp_2(i);
    }
}

void CGame::serp_from_dest(uint64_t bb, uint16_t ** movelist, uint8_t dest_square) {
    int i;
    while(bb) {
        i = bitscan(bb);
        if((dest_square < 8) || (dest_square > 55)) {
            *((*movelist)++) = encode_move(i, dest_square, 4);
            *((*movelist)++) = encode_move(i, dest_square, 3);
            *((*movelist)++) = encode_move(i, dest_square, 2);
            *((*movelist)++) = encode_move(i, dest_square, 1);
        } else {
            *((*movelist)++) = encode_move(i, dest_square, 0);
        }
        bb &= ~exp_2(i);
    }
}


// ===============================================================================================================
//  These functions are for converting a source, dest and flag to a move or to retrieve those things from a move.
// ===============================================================================================================

uint16_t encode_move(uint16_t source, uint16_t dest, uint16_t special) {
    return source | ((dest << 6) | (special << 12));
}

uint16_t extract_source(uint16_t move) {
    return move & MASK_FROM;
}

uint16_t extract_dest(uint16_t move) {
    return (move & MASK_TO) >> 6;
}

uint16_t extract_special(uint16_t move) {
    return (move & MASK_SPECIAL) >> 12;
}


// =========================================================================
//  Perft and divide are for testing the accuracy of the move_gen function.
// =========================================================================

uint64_t CEngine::divide(CGame * game, char depth, bool quiet_moves) {
    uint64_t perft_nodes = 0, x;

    uint16_t movelist[256];
    if(quiet_moves || game->in_check()) game->generate_moves(movelist);
    else game->generate_caps(movelist);

    for(int i = 0; movelist[i]; i++){
        game->make_move(movelist[i]);
        print_move(movelist[i]);
        x = ((depth >= 2)? (perft(game, depth - 1, quiet_moves) - perft(game, depth-2, quiet_moves)) : 1);
        cout << ": " << x << "\n";
        game->unmake_move();
        perft_nodes += x;
    }
    return perft_nodes;
}

uint64_t CEngine::perft(CGame * game, char depth, bool quiet_moves) {
    uint64_t perft_nodes = 1;

    if(depth == 0) return 1;

    uint16_t movelist[256];
    if(quiet_moves || game->in_check()) game->generate_moves(movelist);
    else game->generate_caps(movelist);

    for(int i = 0; movelist[i]; i++){
        game->make_move(movelist[i]);
        perft_nodes += perft(game, depth - 1, quiet_moves);
        game->unmake_move();
    }

    return perft_nodes;
}

