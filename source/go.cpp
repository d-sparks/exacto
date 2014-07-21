// go.cpp
// created: 9:30PM, 1/2/2014
// author: Daniel Sparks
// license: See text/license.txt
// purpose: Time and search management function go() and engine parameters.

#include "exacto.h"
#include "inlines.h"

/* ===============================================================================================
    go() tells the engine to begin thinking, and (with time management in mind) decide on a move.
   =============================================================================================== */

void CEngine::go(CGame * game) {
    uint16_t        best_move               = BOGUS_MOVE;           // Records the best move from each iteration
    double          dt                      = 0,                    // See below for description.
                    mu                      = 0,                    // mu = average of search returns
                    mu_sq                   = 0,                    // mu_sq = average of squares of search returns
                    s                       = 0,                    // s = variance of search returns
                    relative_instability    = 0,                    // This is calculated after each iteration, see below
                    relative_pressure       = 0,
                    absolute_pressure       = 0;
    int16_t         initial_depth           = 0,                    // The first depth we will search (in the case where of a hash table hit)
                    n                       = 0,                    // n = depths searched consecutively without a change to the root move in the principal variation
                    score                   = 0,
                    epsilon_L               = MATESCORE,
                    epsilon_R               = MATESCORE;
    int             cs_for_turn             = 0;
    uint64_t        estimated_end           = 0,
                    time_ideal_end          = 0,
                    nodes_prev              = 0;
    bool            have_posted             = false;

    force = false;

    if(engine_color == UNASSIGNED)               engine_color = (uint8_t)game->wtm;
    else if(engine_color != (uint8_t)game->wtm)  engine_color = SWITCHED;

    if(post && post_pretty) cout << endl << endl << "Dpth Scor Time  Nodes      Principal Variation" << endl << "---- ---- ----- ---------- ----------------------------------------------------" << endl;

    // --------------------------------------------------------------
    //  (1) Check if only one move is legal, or if the game is over.
    // --------------------------------------------------------------

    do {
        uint16_t movelist[256];
        game->generate_moves(movelist);

        if(movelist[0] == 0) {
            cout << "Game over: " << (game->in_check() ? "CHECKMATE" : "DRAW") << endl;
            return;
        } else if(movelist[1] == 0) {
            game->make_move(movelist[0]);
            cout << "move "; print_move(movelist[0]); cout << endl;
            return;
        }

    // -----------------------------------------------------------
    //  (2) Look at the opening book, and see if a move is legal.
    // -----------------------------------------------------------

        if(book_mode != BOOK_DISABLED) {
            uint16_t book_move, unused;

            if(post) print_book_moves(game);

            find_best_stats(game, book_mode, &book_move, &unused, &unused, &unused);

            if(book_move != BOGUS_MOVE) {
                game->make_move(book_move);
                cout << "move "; print_move(book_move); cout << endl;
                return;
            }
        }

    } while(false);

    // ----------------------------------------------------------------------
    //  (3) Get an idea of how much time to use (or set exact search timer.)
    // ----------------------------------------------------------------------

    time_begin_search = clock();

    if(time_use_exact) {
        cs_for_turn = time_levels[2] * 100;
        time_end_search = time_begin_search + (uint64_t)((5 * time_levels[2] - 1) * (CLOCKS_PER_SEC / 5));
        time_ideal_end = time_end_search;
    } else {

        /*  First we decide on an ideal amount of search time, which is a fraction of the time we have left.  Then we add or subtract from this amount based
            on the amount of surplus time from previous turns (if we ran under or over) and also, we grant ourselves the full amount of time increment.  At
            absolute most, we allow the use of 1/5 of the time remaining.  The bonus for surplus is capped at 1/10 of time remaining, and if we are running
            a negative surplus, we will at worst cut our search time in half.

                   time_left
                --------------- + increment + other = time alloted for this turn, where n = move number.  Max at n = 20 (giving time_left / 30 + ...).
                         400n
                40 -  ---------
                      n^2 + 400

            The denominator (as a function of n) goes through (0, 40), has (20, 30) as a local minimum, and a horizontal asymptote y = 40.      */

        do {
            int padded_cs_left = (time_cs_left > 200) ? time_cs_left - 100 : time_cs_left / 2;

            if(time_levels[0])      cs_for_turn = 2 * padded_cs_left / (time_levels[0] + 5 - ((game->move_num / 2) % time_levels[0]));
            else                    cs_for_turn = (padded_cs_left) / (40 - ((400 * game->move_num)) / ((game->move_num * game->move_num) + 400));

            time_end_search = ((cs_for_turn + ((padded_cs_left - 100) / 50)) * (CLOCKS_PER_SEC / 100)) + clock();
        } while(false);

        if(time_surplus > 0)    cs_for_turn += time_surplus / 3;
        else                    cs_for_turn -= min(time_surplus / 3, cs_for_turn / 2);

        if(time_cs_left > (3 * cs_for_turn) + (2 * (100 * time_levels[2]))) cs_for_turn += 100 * time_levels[2];    // Note: I would like a less discrete version

        time_ideal_end = (cs_for_turn * (CLOCKS_PER_SEC/100)) + clock();


        /*  dt = delta t = (time_cs_left - time_o_cs_left)/max(1, time_levels[1] * 6000, time_cs_left, time_o_cs_left)
            Indicates relative time pressure, ranging from -1 (maximal pressure) to 1 (minimal pressure).   */

        dt = (time_cs_left - time_o_cs_left) / max(time_levels[1] * 6000, max(time_cs_left, time_o_cs_left));

        /*  relative_pressure is a double between 0 and 1/2, which signifies how much more time we have left than our opponents,
            and (crudely) how relevant that difference is.  It is a product of two factors:
                (1) min(0.5, 1.0 - %opponenttimeleft)
                (2) (dt+1)/2
            The first factor will be nearly 0 when our opponent has a large % of their time available, and if the opponent has more than half of their time,
            it will be nearly 1.  The second factor ranges from (0, 1), where a 0 indicates that we have relatively less time than our opponent, and
            1 signifies the opposite.   */

        relative_pressure = min((double)1, 1 - ((double)time_o_cs_left / (time_levels[1] * 6000))) * ((dt + 1) / 2);

        /*  absolute_pressure measures our time surplus as a fraction.  It will be
                0     if the time_surplus is negative, and greater than or equal to 1/4 of time_cs_left
                1/2   if the time surplus is 0
                1     if the time surplus is positive, and greater than or equal to 1/4 of time_cs_left     */

        absolute_pressure = min((double)1, max((double)0, ((double)10 * time_surplus / time_cs_left) + 0.5));
    }



    // ----------------------------------------
    //  (4) Other preparations for the search.
    // ----------------------------------------

    nodes = 0;
    root_material = game->material_balance;
    terminate_search = false;
    hash.clear_repititions(game, 2);

    switch(hash.probe(game->zobrist_key, 0)) {
    case HASH_EXACT:
        initial_depth = ((hash.old_depth_flag[game->zobrist_key % hash.size] & HASH_DEPTH) >> 2) - 1;
    default:
        score = hash.value[game->zobrist_key % hash.size];
        best_move = hash.sugg[game->zobrist_key % hash.size];
        if(hash.probe(game->zobrist_key, 0) == HASH_ALPHA) epsilon_L = MATESCORE;
        else epsilon_L = (MATESCORE + ((int32_t)100 * initial_depth * initial_depth * initial_depth)) / (1 + (initial_depth * initial_depth * initial_depth));
        if(hash.probe(game->zobrist_key, 0) == HASH_BETA) epsilon_R = MATESCORE;
        else epsilon_R = (MATESCORE + ((int32_t)100 * initial_depth * initial_depth * initial_depth)) / (1 + (initial_depth * initial_depth * initial_depth));
        if(score > MATESCORE - epsilon_R) epsilon_R = MATESCORE - score;
        if(score < epsilon_L - MATESCORE) epsilon_L = MATESCORE + score;
    }

    do {                        /* Note: this code is just in case of a type 1 collision. */
        uint16_t movelist[256];
        bool bad_entry = true;
        game->generate_moves(movelist);
        for(uint8_t i = 0; movelist[i]; i++) if(movelist[i] == best_move) bad_entry = false;
        if(bad_entry) best_move = movelist[0];
    } while(false);



    // ----------------------------------------
    //  (5) The main iterative deepening loop.
    // ----------------------------------------

    for(uint8_t i = 1; initial_depth + i < 64; i++) {


        //  (5.I)   --- Statistics / search preparation ---
        nodes_prev = nodes;

        do {
            double checks_per_sec = max((double)2, 30 * pow((double)2 / 3, (double)time_cs_left / 100));
            double nodes_per_sec = (double)(total_cs_used? 100 * total_nodes / total_cs_used : (uint64_t)1500000);
            nodes_per_time_check = (checks_per_sec == 0)? 0 : (uint64_t)floor(nodes_per_sec/checks_per_sec);
            nodes_per_time_check = min((uint64_t)3000000, max((time_cs_left < 6000)? (uint64_t)50000 : (uint64_t)600000, nodes_per_time_check));
        } while(false);

        nodes_next_time_check = nodes + nodes_per_time_check;
        check_ext_count = 0;
        recap_ext_count = 0;
        passed_ext_count = 0;
        ext_plies = 0;
        in_check = 0;

        //  (5.III) --- Perform the next search iteration ---

#ifdef _ASPIRATION_WINDOW
        score = pvs_root(game, score - epsilon_L, score + epsilon_R, initial_depth + i);
#else
        score = pvs_root(game, -MATESCORE, MATESCORE, initial_depth + i);
#endif

        if(terminate_search) break;

        //  (5.IV)  --- Check for fail high / fail low ---

#ifdef _ASPIRATION_WINDOW
        if(hash.probe(game->zobrist_key, initial_depth + i) == HASH_ALPHA) {
            if(post && (score > -NEARMATE) && (score < NEARMATE) && (have_posted = true))
                post_out(game, initial_depth + i, score, '-');

            epsilon_L = ROOK_VAL;               // Fail low
            epsilon_R = 75;
            if(score > MATESCORE - epsilon_R) epsilon_R = MATESCORE - score;
            if(score < epsilon_L - MATESCORE) epsilon_L = MATESCORE + score;

            if(!time_use_exact) {
                time_ideal_end += min((uint64_t)cs_for_turn * CLOCKS_PER_SEC / 100, (uint64_t)ceil(branching_factor) * (clock() - time_begin_search));
                time_end_search += (time_cs_left / 30) * (CLOCKS_PER_SEC / 100);
            }

            score = pvs_root(game, score - epsilon_L, score + epsilon_R, initial_depth + i);
            if(terminate_search) break;
        } else if(hash.probe(game->zobrist_key, initial_depth + i) == HASH_BETA) {
            if(post && (score > -NEARMATE) && (score < NEARMATE) && (have_posted = true))
                post_out(game, initial_depth + i, score, '+');

            epsilon_L = 75;     // Fail high
            epsilon_R = ROOK_VAL;
            if(score > MATESCORE - epsilon_R) epsilon_R = MATESCORE - score;
            if(score < epsilon_L - MATESCORE) epsilon_L = MATESCORE + score;

            best_move = hash.sugg[game->zobrist_key % hash.size];

            if(!time_use_exact)
                time_ideal_end += min((uint64_t)cs_for_turn * CLOCKS_PER_SEC / 100, (uint64_t)ceil(branching_factor) * (clock() - time_begin_search));

            score = pvs_root(game, score - epsilon_L, score + epsilon_R, initial_depth + i);
            if(terminate_search) break;
        }
#endif

        //  (5.Y)  --- Update some statistics ---

        mu = ((mu * (i-1)) + score) / i;
        mu_sq = ((mu_sq * (i-1)) + (score * score)) / i;
        s = mu_sq - mu * mu;

        if(hash.probe(game->zobrist_key, initial_depth + i) == HASH_EXACT) {
            if(hash.PV_move[game->zobrist_key % PV_size] == best_move) n++;
            else best_move = hash.PV_move[game->zobrist_key % PV_size];
            relative_instability = (min(s/5, (double)1)*(1 - ((double)n/i)));
        } else
            relative_instability = 1;

        if((initial_depth == 0) && (i == 3) && (nodes_prev > 1))
            branching_factor = min((double)5, max((double)1, (double)nodes / nodes_prev));
        else if((i >= 3) && (nodes_prev > 1))
            branching_factor = min((double)5, max((double)1, (i * branching_factor + ((i/2)*(double)nodes / nodes_prev)) / (i + i/2)));


        //  (5.X)  --- Update aspiration windows ---


        /* Let d denote the initial depth, P = PAWN_VAL, M = MATESCORE, then, if this is the first iteration, we take our window to be

                         M + 200dP              M + 200dP
            ( score  -  ---------- , score  +  ----------- )
                         200d + 1               200d + 1

        if we are merely updating our windows, we use a weighted average to normalize towards N = (0.4I + 0.1)*PAWN_VAL, where I is the
        relative_instability invariant.  Thus, N \in (PAWN_VAL/10, PAWN_VAL/2), depending on instability.  Positions which are more stable
        correspond to I being nearly 0, which is to say that N will be close to PAWN_VAL/10.  So, if E_l, E_r are the previous margins

                         4N + E_l               4N + E_r
            ( score  -  ---------- , score  +  ---------- )
                            5                      5
        */

#ifdef _ASPIRATION_WINDOW
        if(i == 1) {
            epsilon_L = (MATESCORE + (50 * initial_depth * PAWN_VAL)) / (200 * initial_depth + 1);
            epsilon_R = (MATESCORE + (50 * initial_depth * PAWN_VAL)) / (200 * initial_depth + 1);
        } else {
            uint16_t normalize_towards = (uint16_t)((0.1 + 0.4 * relative_instability) * PAWN_VAL);
            if(score > -NEARMATE) epsilon_L = (2 * normalize_towards + epsilon_L) / 3;
            if(score < NEARMATE) epsilon_R = (2 * normalize_towards + epsilon_R) / 3;
        }
        if(score > MATESCORE - epsilon_R) epsilon_R = MATESCORE - score;
        if(score < epsilon_L - MATESCORE) epsilon_L = MATESCORE + score;
#endif

#ifdef _HIDE_EARLY_POST
        if(post && ((initial_depth != 0) || (20*(clock() - time_begin_search) > (time_ideal_end - time_begin_search))
                                         || ((clock() - time_begin_search)/(CLOCKS_PER_SEC/100) > 25)
                                         || have_posted)
                && (have_posted = true))
#endif
            post_out(game, initial_depth + i, score, ' ');

        //  (5.VI)  --- Determine whether to iterate ---

        if((abs(score) > NEARMATE) && (hash.probe(game->zobrist_key, 0) == HASH_EXACT)) break;

        uint64_t cur_clock = clock();
        estimated_end = (uint64_t)ceil(branching_factor + 0.6) * (cur_clock - time_begin_search) + time_begin_search;

        if(!time_use_exact && (estimated_end > time_ideal_end))
            if((1 - ((double)(time_ideal_end - cur_clock) / (estimated_end - cur_clock))) > ((absolute_pressure + relative_pressure + relative_instability)/3))
                break;

    }

#ifdef _HIDE_EARLY_POST
    if(post && !have_posted) post_out(game, (hash.old_depth_flag[game->zobrist_key % hash.size] & HASH_DEPTH) >> 2, score, ' ');
#endif


    // ----------------------------------
    //  (6) Update some more statistics.
    // ----------------------------------

    total_nodes += nodes;
    total_cs_used += ((int64_t)clock() - time_begin_search)/(CLOCKS_PER_SEC/100);
    time_cs_left += (100 * time_levels[2]) - (int)(((int64_t)clock() - time_begin_search)/(CLOCKS_PER_SEC/100));
    time_surplus += (int)((int64_t)time_ideal_end - (int64_t)clock())/(CLOCKS_PER_SEC/100);

    // ----------------------------------------------
    //  (6) Make the move and resign (if necessary).
    // ----------------------------------------------

    //uint16_t movelist[256];
    game->make_move(best_move);
    //game->generate_moves(movelist);

    bool losing = (score < (-40 - 40 * (((double)game->number_of_pieces[WHITE] + game->number_of_pieces[BLACK]) / 14)));

    if(game->is_drawn() /*|| (!game->in_check() && (movelist[0] == 0)) */ || (draw_offered && losing)) {
        cout << "offer draw" << endl;
    }  else
        draw_offered = false;

    cout << "move "; print_move(best_move); cout << endl;

}
