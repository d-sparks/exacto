// game.h
// created: 12:23AM 1/2/2014
// author: Daniel Sparks
// license: See text/license.txt
// purpose: Header file for the gamestate class.

#pragma once

#include "includes.h"

class CGame {
public:

/*  Game mechanics.     */
    void            set_board                   (string brd, string clr, string cstl, string ep, string hm, string fm);
    void            make_move                   (uint16_t move);
    void            unmake_move                 ();
    void            make_null                   ();
    void            unmake_null                 ();

/*  Positional evaluation.  (See evaluate.cpp.) */

    int16_t         qevaluate                   (int16_t alpha, int16_t beta, uint64_t SEE_bbs[2][7]);
    int16_t         evaluate                    (int16_t alpha, int16_t beta);
    int16_t         evaluate_material           ();
    int16_t         evaluate_pawn_structure     ();
    bool            is_drawn                    ();
    bool            repeated                    (uint16_t ply);
    bool            repeated_or_50m             (uint16_t ply);
    bool            have_piece                  ();
    bool            is_promoting                ();
    bool            is_passed_pawn_move         (uint16_t move);


/*  Hash table interaction. */

    void            get_zobrist_keys            ();
    bool            clear_repititions           (uint8_t depth);
    void            new_pawn_hash               (int value);
    uint64_t        hash_key_pawns              ();
    bool            probe_pawn_hash             ();


/*  Move generation.    */

    void            cap_gen                     (uint16_t * movelist, uint64_t SEE_bbs[2][7]);
    void            generate_caps               (uint16_t * movelist);
    void            move_gen                    (uint16_t * movelist, uint64_t SEE_bbs[2][7]);
    void            generate_moves              (uint16_t * movelist);
    void            attack_set_gen              (bool color, uint64_t SEE_bbs[2][7]);
    void            attack_set_gen_light        (bool color, uint64_t * bb);
    bool            in_check                    ();
    uint16_t        get_prev_move               ();

/*  SEE support.    */

    int16_t         SEE                         (uint8_t source, uint8_t dest);
    int16_t         SEE_next                    (int16_t prev_piece, uint8_t square, bool color);
    uint8_t         find_least_attacker         (bool color, uint8_t dest);
    void            SEE_make                    (bool color, uint8_t source, uint8_t dest);
    void            SEE_unmake                  (bool color, uint8_t attacker, uint8_t source, uint8_t dest);

/*  UI / Debugging.     */

    void            print_board                 ();
    void            print_move_history          ();
    string          get_fancy_move              (uint16_t move);
    void            print_FEN                   ();

    void            print_all_bitboards         ();
    void            print_move_history_i        ();
    bool            is_corrupt                  (uint64_t the_pieces[2][7]);
    void            store_pieces                (uint64_t the_pieces[2][7]);
    void            freeze                      (uint16_t move, uint64_t local_pieces[2][7]);
    void            print_evaluate              ();


    CGame(void);
    ~CGame(void);


/*  Gamestate data. */
    uint64_t        pieces[2][7],               en_passant,                 castling[2],                zobrist_key,
                    occupied,                   empty,                                                  zobrist_key_pawns;
    uint8_t         board[64];
    bool            wtm,                        btm;

/*  Evaluation data.    */

    int16_t         material_balance,           incremental_positional;
    uint8_t         open_files[2],              passed_pawns[2],            weak_pawns[2];
    char            number_of_pieces[2],        number_of_pawns[2];
    int16_t         num_of_pieces,              num_of_pawns;

/*  Game history.   */

    uint16_t        move_num,                   hist_moves[1024];
    uint64_t        hist_epc[1024],             hist_zobrist[1024];
    uint8_t         hist_defender[1024],        hist_50m[1024];

private:

/*  Serialization functions for move generation.    */

    void            serp_w                      (uint64_t bb, uint16_t ** movelist);
    void            serp_w_d                    (uint64_t bb, uint16_t ** movelist);
    void            serp_w_capleft              (uint64_t bb, uint16_t ** movelist);
    void            serp_w_capright             (uint64_t bb, uint16_t ** movelist);
    void            serp_b                      (uint64_t bb, uint16_t ** movelist);
    void            serp_b_d                    (uint64_t bb, uint16_t ** movelist);
    void            serp_b_capleft              (uint64_t bb, uint16_t ** movelist);
    void            serp_b_capright             (uint64_t bb, uint16_t ** movelist);
    void            serp_ep                     (uint64_t bb, uint16_t ** movelist, uint8_t ep_square);
    void            serialize_piece             (uint64_t bb, uint16_t ** movelist, uint8_t source_square);
    void            serialize_castling          (uint64_t bb, uint16_t ** movelist);
    void            serialize_from_dest         (uint64_t bb, uint16_t ** movelist, uint8_t dest_square);
    void            serp_d_from_dest            (uint64_t bb, uint16_t ** movelist, uint8_t dest_square);
    void            serp_ep_from_dest           (uint64_t bb, uint16_t ** movelist, uint8_t dest_square);
    void            serp_from_dest              (uint64_t bb, uint16_t ** movelist, uint8_t dest_square);

};