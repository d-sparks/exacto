// engine.h
// created: 1:17AM, 2/3/2014
// author: Daniel Sparks
// license: See text/license.txt
// purpose: Header file for the engine class.

#pragma once

#include "definitions.h"
#include "includes.h"
#include "game.h"
#include "hash.h"

class CEngine {
public:

/*  Go (see go.cpp).    */
    void            go                          (CGame * game);

/*  Search algorithms (see search.cpp).    */

    int16_t         pvs_root                    (CGame * game, int16_t alpha, int16_t beta, uint8_t depth);
    int16_t         pvs                         (CGame * game, int16_t alpha, int16_t beta, uint8_t depth, uint16_t ply);
    int16_t         alphabeta                   (CGame * game, int16_t alpha, int16_t beta, uint8_t depth, uint16_t ply);
    int16_t         qsearch                     (CGame * game, int16_t alpha, int16_t beta, uint16_t ply);
    uint8_t         R_ad                        (CGame * game, uint8_t depth);

/*  Move ordering (see sort.cpp).   */
    void            order_moves                 (CGame * game, uint16_t * movelist, int16_t * sortlist, uint8_t ply);
    void            order_caps                  (CGame * game, uint16_t * movelist, int16_t * sortlist, uint64_t SEE_bbs[2][7]);
    void            assign_values               (CGame * game, uint16_t * movelist, int16_t * sortlist, uint8_t ply);
    void            assign_cap_values           (CGame * game, uint16_t * movelist, int16_t * sortlist, uint64_t SEE_bbs[2][7]);
    void            sort_remaining              (uint16_t * movelist, int16_t * sortlist, uint8_t leftmost);
    int16_t         fuzzy_SEE                   (uint64_t SEE_bbs[2][7], int prev_capper, uint8_t square, bool color);
    void            update_killers              (uint8_t depth, uint16_t move);

/*  Perft and divide (see movegen.cpp).   */

    uint64_t        divide                      (CGame * game, char depth, bool quiet_moves);
    uint64_t        perft                       (CGame * game, char depth, bool quiet_moves);

/*  Winboard/XBoard time input commands (see cengine.cpp).  */

    void            st                          (int seconds);
    void            set_levels                  (int MPS, int base, int inc);
    void            reset_engine                ();
    void            time                        (int centiseconds);
    void            otim                        (int centiseconds);

/*  I/O (see io.cpp).   */

    void            print_pv                    (CGame * game, uint8_t depth, uint8_t ply, uint16_t current_move, uint8_t cursor_pos);
    void            print_engine_stats          ();
    void            post_out                    (CGame * game, uint8_t depth, int score, char high_low_symb);
    void            print_EVALUATE              ();

/*  Getters/setters.    */

    bool            get_force                   ();


    CEngine(void);
    ~CEngine(void);

/*  Move ordering heuristics.   */

    uint64_t        hh[2][4096],                hh_best[2];
    uint16_t        killer_move[2][128],        killer_counter[2][128];
    int16_t         sort[256][256];

/*  Each CEngine has an attached CHash (hash table).    */

    CHash           hash;

/*  Engine profiling and time management.   */

    string          opponent_name;
    double          branching_factor;
    uint64_t        nodes,                      total_nodes,                nodes_next_time_check,      nodes_per_time_check,
                    time_begin_search,          time_end_search,            total_cs_used,              ext_plies,
                    in_check;;
    int             time_cs_left,               time_o_cs_left,             time_levels[3],             time_surplus;
    int16_t         root_material;
    uint8_t         mt,                         check_ext_count,            recap_ext_count,            passed_ext_count,
                    engine_color,               book_mode,                  razor_balance;
    bool            post,                       force,                      time_use_exact,             terminate_search,
                    post_pretty,                draw_offered;

private:

};