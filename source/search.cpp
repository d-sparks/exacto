// search.cpp
// created: 1:17AM AM 12/15/13
// author: Daniel Sparks
// license: See text/license.txt
// purpose: Main search algorithms.

#include "exacto.h"
#include "inlines.h"

int16_t RAZOR_MARGIN = 80; uint8_t RAZOR_MAX_DEPTH = 6;

/* ================================================
    PVS algorithm (alpha-beta variant, fail soft).
   ================================================ */

int16_t CEngine::pvs_root(CGame * game, int16_t alpha, int16_t beta, uint8_t depth) {

    uint64_t        SEE_bbs[2][7]   = {0};
    int16_t         score,
                    best_score      = -INFTY,
                    sortlist[256];
    uint16_t        movelist[256],
                    best_move       = BOGUS_MOVE;
    uint8_t         hash_type       = HASH_ALPHA;
    bool            pre_pv          = true;

    nodes++;

/*  Generate moves and check whether the game is over.  */

    game->move_gen(movelist, SEE_bbs);
    if((movelist[0] == 0) && game->in_check())  return -MATESCORE;  /* Checkmate */
    else if(movelist[0] == 0)                   return  DRAWSCORE;  /* Stalemate */

    if(game->is_drawn())                        return  DRAWSCORE;  /* Draw by 50 move rule or threefold repitition */

#ifdef _HASH_PRUNING

/*  Probe the transposition table.  */

    if(!game->repeated(game->move_num)) {
        switch(hash.probe(game->zobrist_key, depth)) {
        case HASH_BETA:  if(hash.value[game->zobrist_key % hash.size] >= beta)  return hash.value[game->zobrist_key % hash.size]; break;    /* Lower bound ==> fail high. */
        case HASH_ALPHA: if(hash.value[game->zobrist_key % hash.size] <= alpha) return hash.value[game->zobrist_key % hash.size]; break;    /* Upper bound ==> fail low. */
        case HASH_EXACT:                                                        return hash.value[game->zobrist_key % hash.size];           /* Exacto search ==> transposition. */
        }
    }

#endif

/*  Move ordering heuristics.   */

    order_moves(game, movelist, sortlist, 0);

#ifdef _KILLER_MOVES
    killer_move[0][1]     =   0;    killer_move[1][1]     =    0;
    killer_counter[0][1]  =   0;    killer_counter[1][1]  =    0;
#endif

/*  The main PVS algorithm. */

    for(uint8_t i = 0; movelist[i]; i++) {

        game->make_move(movelist[i]);
        game->in_check()? in_check |= exp_2(1) : in_check &= ~exp_2(1);

        if(pre_pv) {
            score = -pvs(game, -beta, -alpha, depth - 1, 1);
        } else {
            score = -pvs(game, -alpha-1, -alpha, depth - 1, 1);
            if(score > alpha) score = -pvs(game, -beta, -alpha, depth - 1, 1);
        }

        game->unmake_move();

        if(terminate_search) return alpha;

        if(score > best_score) {
            best_move = movelist[i];
            best_score = score;
            if(score > alpha) {
                alpha = score;
                hash_type = HASH_EXACT;
                pre_pv = false;
                if((depth > 1) && (i > 0)) {
                    hash.delete_entry(game->zobrist_key);
                    hash.record(game->zobrist_key, movelist[i], depth, HASH_BETA, best_score);
                    if(post) post_out(game, depth, alpha, ' ');
                }
            }
        }

        if(score >= beta) {
            hash.record(game->zobrist_key, movelist[i], depth, HASH_BETA, score);
            return score;
        }

    }

    hash.record(game->zobrist_key, best_move, depth, hash_type, best_score);
    return best_score;

}


/* ================================================
    PVS algorithm (alpha-beta variant, fail soft).
   ================================================ */

int16_t CEngine::pvs(CGame * game, int16_t alpha, int16_t beta, uint8_t depth, uint16_t ply) {

/*  I picked this idea up from Crafty: every N nodes, we look at the clock.  N varies with
    NPS - generally amounting to 2-3 checks per second, and up to 20 or so in situations of
    extreme time pressure.  This is only checked in depths 0-2.  See go.cpp.                    */

    if(nodes > nodes_next_time_check) {
        if(clock() > time_end_search) {
            terminate_search = true;
            return alpha;
        } else
            nodes_next_time_check = nodes + nodes_per_time_check;
    }

/*  I separate the depth 0, 1, and 2 nodes into another function, which has futility pruning
    enabled and the PVS mechanism disabled.  See below.                                         */

    if(depth <= 2) return alphabeta(game, alpha, beta, depth, ply);

    nodes++;


/*  First check if the game is over due to draw by fifty move rule or threefold repitition.
    In fact, we will return the drawscore even if we are merely in the situation of two-fold
    repitition within the current search tree.                                                  */

    if(game->repeated_or_50m(ply)) return DRAWSCORE;


#ifdef _HASH_PRUNING

/*  Pruning based on transposition table entries.                                               */

    switch(hash.probe(game->zobrist_key, depth)) {
        case HASH_BETA:  if(hash.value[game->zobrist_key % hash.size] >= beta)  return hash.value[game->zobrist_key % hash.size]; break;    /* Lower bound ==> fail high. */
        case HASH_ALPHA: if(hash.value[game->zobrist_key % hash.size] <= alpha) return hash.value[game->zobrist_key % hash.size]; break;    /* Upper bound ==> fail low. */
        case HASH_EXACT:                                                        return hash.value[game->zobrist_key % hash.size];           /* Exacto search ==> transposition. */
    }

#endif



    int16_t         score;
    bool            red             = false,    /* This variable is used for two separate things (LMR, and repition detection). */
                    check_ext       = false,
                    recap_ext       = false,
                    passed_ext      = false,
                    fprune          = false,
                    razor           = false,
                    unrazor         = false;

#ifdef _RAZORING

/*  These are the conditions to consider razoring.  */

    if((depth <= RAZOR_MAX_DEPTH)   && (game->evaluate_material() + (RAZOR_MARGIN * (depth - 2)) < alpha)
                                    && ((in_check & exp_2((uint8_t)ply)) == 0)
                                    && ((ext_plies & exp_2((uint8_t)ply - 1)) == 0)
                                    && (alpha < NEARMATE) && (beta > -NEARMATE)) {

        razor_balance++;
        razor = true;
        depth--;

    } else
#endif
#ifdef _FUTILITY_PRUNING

     if((depth <= _FUT_MAX_DEPTH)   && (game->evaluate_material() + (_FUT_MARGIN * depth) < alpha)
                                    && ((in_check & exp_2((uint8_t)ply)) == 0)
                                    && (alpha < NEARMATE) && (beta > -NEARMATE))
        fprune = true;

    else
#endif

#ifdef _NULL_MOVE

/*  If we are not going to attempt razoring or futility pruning, we consider performing the null move forward
    pruning technique.  Specifically, we use adaptive null move pruning, a la Heinz.            */

    if(game->have_piece()   && ((in_check & exp_2((uint8_t)ply)) == 0)
                            && (game->get_prev_move() != NULL_MOVE)
                            && ((hash.probe(game->zobrist_key, depth - 3) != HASH_ALPHA) || (hash.value[game->zobrist_key % hash.size] >= beta))) {

        killer_move[0][ply + 1] = 0; killer_counter[0][ply + 1] = 0;
        killer_move[1][ply + 1] = 0; killer_counter[1][ply + 1] = 0;

        game->make_null();
        in_check &= ~exp_2(ply + 1);
        score = -pvs(game, -beta, -beta + 1, depth - R_ad(game, depth), ply + 1);
        game->unmake_null();
        if(score >= beta)
            return score;

    }

#endif

#ifdef _IID

/*  Internal iterative deepening is used when depth is sufficiently large, but there is
    no hash table suggestion for the best move.  Note: I picked up the idea of requiring
    that beta - alpha > 1 from Crafty.                                                          */

    if((beta - alpha > 1) && (hash.probe(game->zobrist_key, 0) != HASH_EXACT) && ((in_check & exp_2((uint8_t)ply)) == 0))
        pvs(game, alpha, beta, depth - 2, ply);

#endif

/*  Generate moves, and check whether the game is over due to checkmate or stalemate.           */





    uint8_t         hash_type = HASH_ALPHA;
    uint64_t        SEE_bbs[2][7] = {0};
    uint16_t        movelist[256];
    int16_t         sortlist[256];

    game->move_gen(movelist, SEE_bbs);

    if(movelist[0] == 0) {
        if(in_check & exp_2((uint8_t)ply))  return -MATESCORE + (int16_t)ply;   /* Checkmate */
        else                                return  DRAWSCORE;                  /* Stalemate */
    }

/*  Move ordering heuristics.  See sort.cpp for details on move ordering.  Ultimately,
    hash suggestions, SEE, MVV/LVA, killer moves and history heuristics (in the lower
    plies) are all used.                                                                        */

    order_moves(game, movelist, sortlist, (uint8_t)ply);

#ifdef _KILLER_MOVES
    killer_move[0][ply + 1] = 0; killer_counter[0][ply + 1] = 0;
    killer_move[1][ply + 1] = 0; killer_counter[1][ply + 1] = 0;
#endif

#ifdef MAX_PASSED_PAWN_EXTENSIONS
    game->evaluate_pawn_structure();
#endif

    int16_t         best_score      = -INFTY;
    uint16_t        best_move       = BOGUS_MOVE;


/*  The PVS algorithm.  */

    RE_SEARCH:

    for(uint8_t i = 0; movelist[i]; i++) {

#ifdef _LMR

    /*  Late move reductions: in likely fail-low scenarios, we opt to search unpromising
        moves at reduced depth.                                                                 */

        if((depth >= LMR_MIN_DEPTH) && (hash_type == HASH_ALPHA)
                                    && (i >= LMR_FULL_MOVES)
                                    && ((in_check & exp_2((uint8_t)ply)) == 0)
                                    && (sortlist[i] < SORT_KILLER)
                                    && ((ext_plies & exp_2(ply-1)) == 0))
                red = true;

#endif

        game->make_move(movelist[i]);
        game->in_check()? in_check |= exp_2(ply + 1) : in_check &= ~exp_2(ply + 1);

    /*  Search extensions */

#ifdef MAX_CHECK_EXTENSIONS
        if(in_check & exp_2(ply + 1)) {
            if(check_ext_count < MAX_CHECK_EXTENSIONS) {
                check_ext = true;
                check_ext_count++;
                ext_plies |= exp_2((uint8_t)ply);
            }
            red = false;
        }

#endif
#ifdef MAX_RECAP_EXTENSIONS
        if((beta > alpha + 1) && (game->hist_defender[game->move_num] != 0)
                              && (game->hist_defender[game->move_num - 1] != 0)
                              && (game->material_balance == root_material)
                              && (extract_dest(game->get_prev_move()) == extract_dest(movelist[i]))) {
            if(recap_ext_count < MAX_RECAP_EXTENSIONS) {
                recap_ext = true;
                recap_ext_count++;
                ext_plies |= exp_2((uint8_t)ply);
            }
            red = false;
            if(razor_balance) {
                depth++;
                unrazor = true;
            }
        }

#endif
#ifdef MAX_PASSED_PAWN_EXTENSIONS
        if(game->is_passed_pawn_move(movelist[i])) {
            if(passed_ext_count < MAX_PASSED_PAWN_EXTENSIONS) {
                passed_ext = true;
                passed_ext_count++;
                ext_plies |= exp_2((uint8_t)ply);
            }
            red = false;
        }
#endif

#ifdef _FUTILITY_PRUNING

    /*  Futility pruning.   */

        if(fprune && (i > 0)
                  && ((in_check & exp_2((uint8_t)ply)) != 0)
                  && (game->evaluate_material() + (sortlist[i] - SORT_GOOD_CAP) + (FP_MARGIN * depth) < alpha)
                  && ((ext_plies & exp_2(ply - 1)) == 0)) {
            game->unmake_move();
            continue;
        }

#endif

    /*  PVS recursion.  */

        if(i == 0)
            score = -pvs(game, -beta, -alpha, depth + (check_ext | recap_ext | passed_ext) - 1, ply + 1);
        else {
            if(red) score = -pvs(game, -alpha - 1, -alpha, depth - 2, ply + 1);
            if((!red) || (score > alpha)) {
                score = -pvs(game, -alpha - 1, -alpha, depth + (check_ext | recap_ext | passed_ext) - 1, ply + 1);
                if((score > alpha) && (score < beta))
                    score = -pvs(game, -beta, -alpha, depth + (check_ext | recap_ext | passed_ext) - 1, ply + 1);
            }
        }

        game->unmake_move();

    /*  Undo the extensions.    */

#ifdef MAX_CHECK_EXTENSIONS
        if(check_ext) {
            check_ext_count--;
            check_ext = false;
            ext_plies &= ~exp_2((uint8_t)ply);
        }
#endif
#ifdef MAX_RECAP_EXTENSIONS
        if(recap_ext) {
            recap_ext_count--;
            recap_ext = false;
            ext_plies &= ~exp_2((uint8_t)ply);
            if(unrazor) {
                razor_balance++;
                unrazor = false;
            }
        }
#endif
#ifdef MAX_PASSED_PAWN_EXTENSIONS
        if(passed_ext) {
            passed_ext_count--;
            passed_ext = false;
            ext_plies &= ~exp_2((uint8_t)ply);
        }
#endif

        if(score >= beta) {

#ifdef _KILLER_MOVES

        /*  Update killer moves.    */

            if(game->board[extract_dest(movelist[i])] == 0)
                update_killers((uint8_t)ply, movelist[i]);

#endif

            hash.record(game->zobrist_key, movelist[i], depth, HASH_BETA, score);

            if(razor) razor_balance--;

            return score;

        }

        if(score > best_score) {
            best_move   =   movelist[i];
            best_score  =   score;
            if(score    >   alpha) {
                alpha       =   score;
                hash_type   =   HASH_EXACT;
            }
        }

    }

    if(razor) {
        razor_balance--;
        if(score > alpha) {
            razor = false;
            depth++;
            goto RE_SEARCH;
        }
    }

    if(!terminate_search)
        hash.record(game->zobrist_key, best_move, depth, hash_type, best_score);
    return best_score;

}


/* ============================================================
    Alpha beta algorithm, fail soft (only used in depths <= 2)
   ============================================================ */

int16_t CEngine::alphabeta(CGame * game, int16_t alpha, int16_t beta, uint8_t depth, uint16_t ply) {

/*  Next, check if the game is over due to draw by fifty move rule or threefold repitition.     */

    if(game->repeated_or_50m(ply)) return DRAWSCORE;
    //if(game->is_drawn()) return DRAWSCORE;

#ifdef _HASH_PRUNING

/*  Probe the transposition table.  */

    switch(hash.probe(game->zobrist_key, depth)) {
        case HASH_BETA:
            if(hash.value[game->zobrist_key % hash.size] >= beta) {     /* A lower bound indicates a fail high. */
                nodes++;
                return hash.value[game->zobrist_key % hash.size];
            }
            break;
        case HASH_ALPHA:
            if(hash.value[game->zobrist_key % hash.size] <= alpha) {    /* An upper bound indicates a fail low. */
                nodes++;
                return hash.value[game->zobrist_key % hash.size];
            }
            break;
        case HASH_EXACT:                                                /* A transposition: we have searched this before. */
            nodes++;
            return hash.value[game->zobrist_key % hash.size];
        break;
    }

#endif

    int16_t futility_margin;
    bool    fprune      = false,
            check_ext   = false,
            recap_ext   = false,
            passed_ext  = false;


    switch(depth) {

    /*  The leaves of the PVS/alphabeta tree are root nodes of quiscence search trees. */

        case 0: return qsearch(game, alpha, beta, ply);

        break;

#ifdef _FUTILITY_PRUNING

    /*  At frontier nodes and pre-frontier nodes, we check if we will attempt futility pruning. */

        case 1: if ((game->evaluate_material() + FP_MARGIN < alpha)     && ((in_check & exp_2((uint8_t)ply)) == 0)
                                                                        && (alpha > -NEARMATE)
                                                                        && (beta < NEARMATE)) {
                    fprune = true;
                    futility_margin = game->evaluate_material() + FP_MARGIN;
                }
        break;
        case 2: if ((game->evaluate_material() + PFNFP_MARGIN < alpha)  && ((in_check & exp_2((uint8_t)ply)) == 0)
                                                                        && (alpha > -NEARMATE)
                                                                        && (beta < NEARMATE)) {
                    fprune = true;
                    futility_margin = game->evaluate_material() + PFNFP_MARGIN;
                }

#endif

    }

    nodes++;


    int16_t         score;

#ifdef _NULL_MOVE

/*  If we are not going to attempt futility pruning, we consider performing the null move
    forward pruning technique.  Specifically, we use adaptive null move pruning, a la Heinz.
    Since alphabeta is only called in depths 0, 1, 2, we go directly to the qsearch.            */

    if (!fprune && ((in_check & exp_2((uint8_t)ply)) == 0) && (game->get_prev_move() != NULL_MOVE) && game->have_piece()) {

        game->make_null();
        in_check &= ~exp_2(ply + 1);
        score = -qsearch(game, -beta, -beta + 1, ply);
        game->unmake_null();
        if(score >= beta)
            return score;

    }

#endif




    uint64_t        SEE_bbs[2][7]   = {0};
    int16_t         best_score      = -INFTY;
    uint16_t        best_move       = BOGUS_MOVE,
                    movelist[256];
    int16_t         sortlist[256];
    uint8_t         hash_type       = HASH_ALPHA;

/*  Generate moves and check if the game is over due to checkmate / stalemate.  */

    game->move_gen(movelist, SEE_bbs);

    if(movelist[0] == 0) {
        if(in_check & exp_2((uint8_t)ply))  return ply - MATESCORE;     /* Checkmate */
        else                                return DRAWSCORE;           /* Stalemate */
    }

/*  Move ordering heuristics.   */

    order_moves(game, movelist, sortlist, (uint8_t)ply);

#ifdef _KILLER_MOVES
    killer_move[0][ply + 1] = 0; killer_counter[0][ply + 1] = 0;
    killer_move[1][ply + 1] = 0; killer_counter[1][ply + 1] = 0;
#endif



/*  The main alphabeta loop.    */

    for(uint8_t i = 0; movelist[i]; i++) {

        game->make_move(movelist[i]);
        game->in_check() ? in_check |= exp_2((uint8_t)ply + 1) : in_check &= ~exp_2((uint8_t)ply + 1);


    /*  Search extensions.  */

#ifdef MAX_CHECK_EXTENSIONS
        if (in_check & exp_2((uint8_t)ply + 1)) {
            if(check_ext_count < MAX_CHECK_EXTENSIONS) {
                check_ext = true;
                check_ext_count++;
                ext_plies |= exp_2((uint8_t)ply);
            }
        }
#endif
#ifdef MAX_RECAP_EXTENSIONS
        if((beta > alpha + 1) && (game->hist_defender[game->move_num] != 0)
                              && (game->hist_defender[game->move_num - 1] != 0)
                              && (game->material_balance == root_material)
                              && (extract_dest(game->get_prev_move()) == extract_dest(movelist[i]))) {
            if(recap_ext_count < MAX_RECAP_EXTENSIONS) {
                recap_ext = true;
                recap_ext_count++;
                ext_plies |= exp_2((uint8_t)ply);
            }
        }
#endif
#ifdef MAX_PASSED_PAWN_EXTENSIONS
        if(game->is_passed_pawn_move(movelist[i])) {
            if(passed_ext_count < MAX_PASSED_PAWN_EXTENSIONS) {
                passed_ext = true;
                passed_ext_count++;
                ext_plies |= exp_2((uint8_t)ply);
            }
        }
#endif

#ifdef _FUTILITY_PRUNING

    /*  Futility pruning.   */

        if (fprune && (i > 0)
                   && ((in_check & exp_2((uint8_t)ply + 1)) == 0)
                   && (futility_margin + sortlist[i] - SORT_GOOD_CAP < alpha)
                   && ((ext_plies & exp_2((uint8_t)ply - 1)) == 0)) {
            game->unmake_move();
            continue;
        }

#endif

        score = -alphabeta(game, -beta, -alpha, (depth + (check_ext | recap_ext | passed_ext)) - 1, ply + 1);

        game->unmake_move();

    /*  Undo the extensions.    */

#ifdef MAX_CHECK_EXTENSIONS
        if(check_ext) {
            check_ext_count--;
            check_ext = false;
            ext_plies &= ~exp_2((uint8_t)ply);
        }
#endif
#ifdef MAX_RECAP_EXTENSIONS
        if(recap_ext) {
            recap_ext_count--;
            recap_ext = false;
            ext_plies &= ~exp_2((uint8_t)ply);
        }
#endif
#ifdef MAX_PASSED_PAWN_EXTENSIONS
        if(passed_ext) {
            passed_ext_count--;
            passed_ext = false;
            ext_plies &= ~exp_2((uint8_t)ply);
        }
#endif

        if(score >= beta) {

#ifdef _KILLER_MOVES

        /*  Update killer moves.    */

            if(game->board[extract_dest(movelist[i])] == 0)
                update_killers((uint8_t)ply, movelist[i]);

#endif

            hash.record(game->zobrist_key, movelist[i], depth, HASH_BETA, score);

            return score;

        }

        if(score > best_score) {
            best_move = movelist[i];
            best_score = score;
            if(score > alpha) {
                alpha = score;
                hash_type = HASH_EXACT;
            }

        }

    }

    hash.record(game->zobrist_key, best_move, depth, hash_type, best_score);
    return best_score;
}


/* ==================================================================================
    Qsearch is an alpha-beta search that only searches exciting moves like captures,
    checks, check evasions and promotions.
   ================================================================================== */

int16_t CEngine::qsearch(CGame * game, int16_t alpha, int16_t beta, uint16_t ply) {

    nodes++;

/*  First check if the game is over due to draw by fifty move rule or threefold repitition.     */

    if(game->is_drawn()) return DRAWSCORE;

    uint64_t SEE_bbs[2][7] = {0};

    int16_t score = game->qevaluate(alpha, beta, SEE_bbs);

    if(score >= beta)
        return score;

    if(alpha < score) alpha = score;

/*  Delta (futility) pruning.   */

#ifdef _DELTA_PRUNING

    int16_t delta = game->is_promoting()?   (2*QUEEN_VAL) - PAWN_VAL : QUEEN_VAL;
    if(score < (alpha - delta)) return alpha;

#endif


/*  Generate captures and moves that give check.  If side to move is in check,
    all legal moves are generated instead.  If no moves are generated and the
    side to move is in check, it is actually checkmate.  Stalemates cannot be
    detected here, however.                                                     */

    uint16_t movelist[100];
    int16_t  sortlist[100];
    bool am_in_check = game->in_check();

    if(am_in_check) {
        game->move_gen(movelist, SEE_bbs);
        if(movelist[0] == 0) return (int16_t)ply - MATESCORE;
        //order_caps(game, movelist, sortlist, SEE_bbs);
        for(uint8_t i = 0; movelist[i]; i++) sortlist[i] = SORT_GOOD_CAP;
    } else {
        game->cap_gen(movelist, SEE_bbs);
        order_caps(game, movelist, sortlist, SEE_bbs);
    }

    for(uint8_t i = 0; (movelist[i] != 0) && (sortlist[i] > SORT_BAD_CAP); i++){
        game->make_move(movelist[i]);
        score = -qsearch(game, -beta, -alpha, ply + 1);
        game->unmake_move();

        if(score >= beta)   return score;

        if(score > alpha)   alpha = score;

    }
    return alpha;
}


// =============================================================
//  This is for adaptive null move forward pruning, a la Heinz.
// =============================================================

uint8_t CEngine::R_ad(CGame * game, uint8_t depth) {
    if(depth >= R_ad_2) return 4;
    else if((depth >= R_ad_1) && (game->number_of_pieces[WHITE] >= 3) && (game->number_of_pieces[BLACK] >= 3)) return 4;
    else return 3;
}