// evalute.cpp
// created: 10:01AM AM 12/15/13
// author: Daniel Sparks
// license: See text/license.txt
// purpose: Static position evaluation functions.

#include "exacto.h"
#include "inlines.h"

int16_t             piece_values[2][7]              = { { 0, -PAWN_VAL, -KNIGHT_VAL, -BISHOP_VAL, -ROOK_VAL, -QUEEN_VAL},
                                                        { 0, PAWN_VAL, KNIGHT_VAL, BISHOP_VAL, ROOK_VAL, QUEEN_VAL } };

int16_t             PST[2][7][64]                   = { 0 };

const int16_t       king_placement_early[64]        = { 10, 20,  4,  0,  4, 20, 20, 10,
                                                         8,  5,  2, -5,  2,  5,  5,  8,
                                                        -5,-15,-25,-30,-30,-15, -15,-5,
                                                       -20,-30,-40,-50,-50,-40,-30,-20,
                                                       -20,-30,-40,-50,-50,-40,-30,-20,
                                                        -5,-15,-25,-30,-30,-15, -15,-5,
                                                         8,  5,  2, -5,  2,  5,  5,  8,
                                                        10, 20,  4,  0,  4, 20, 20, 10};

const int16_t       king_placement_late[64]         = {  0,  0,  0,  5,  5,  0,  0,  0,
                                                         5,  7,  8, 10, 10,  8,  7,  5,
                                                         9, 10, 15, 20, 20, 15, 10,  9,
                                                        10, 15, 20, 30, 30, 20, 15, 10,
                                                        10, 15, 20, 30, 30, 20, 15, 10,
                                                         9, 10, 15, 20, 20, 15, 10,  9,
                                                         5,  7,  8, 10, 10,  8,  7,  5,
                                                         0,  0,  0,  5,  5,  0,  0,  0};



// ==================================================================================
//  Evaluate material returns the material score (negative if black is on the move).
// ==================================================================================

int16_t CGame::evaluate_material() {
    if(wtm)
        return material_balance;
    else
        return -material_balance;
}

// ====================================================================
//  Evaluate calls QEvaluate, but supplies it's own attack set boards.
// ====================================================================

int16_t CGame::evaluate(int16_t alpha, int16_t beta) {
    uint64_t SEE_bbs[2][7] = {0};
    return qevaluate(alpha, beta, SEE_bbs);
}

// ======================================================================================================
//  QEvaluate is a function which, without moving pieces, determines who is winning in a given position.
// ======================================================================================================

int16_t CGame::qevaluate(int16_t alpha, int16_t beta, uint64_t SEE_bbs[2][7]) {

    /*  +-----------------------------------+
        | Phase I                           |
        +-----------------------------------+
        | a. Material                       |
        | b. Piece square tables            |
        | c. Pawn structure                 |
        | d. Bishop pair                    |
        | e. On the move bonus              |
        | f. King's pawn support            |
        +-----------------------------------+   */

    if(is_drawn()) return DRAWSCORE;

    //  --- I.a,b --- Material balance and the piece square tables

    uint64_t        x,
                    y;
    int16_t         early               = 0,
                    late                = 0,
                    fine_evaluation     = material_balance + incremental_positional;
    uint8_t         white_king          = bitscan(pieces[WHITE][KING]),
                    black_king          = bitscan(pieces[BLACK][KING]),
                    i;

    early   +=  king_placement_early[white_king] - king_placement_early[black_king];
    late    +=  king_placement_late[white_king]  - king_placement_late[black_king];

    do {

        //  --- I.c --- Pawn structure evaluation

#ifdef _PAWN_STRUCTURE
        fine_evaluation += evaluate_pawn_structure();
#endif

        //  --- I.d --- Bishop pairs (all: _BISHOP_PAIR)

#ifdef _BISHOP_PAIR
        if(pop_geq_2(pieces[WHITE][BISHOP])) {
            if(!pop_geq_2(pieces[BLACK][BISHOP])) fine_evaluation += _BISHOP_PAIR;
        } else if(pop_geq_2(pieces[BLACK][BISHOP])) fine_evaluation -= _BISHOP_PAIR;
#endif

        //  --- I.e --- On the move bonus (all: _ON_THE_MOVE)

#ifdef _ON_THE_MOVE
        fine_evaluation += wtm ? _ON_THE_MOVE : -_ON_THE_MOVE;
#endif


        //  --- I.f --- King's pawn fortress (early: 30)

        if(pop_leq_1(king_moves[white_king] & pieces[WHITE][PAWN])) {
            if(king_moves[white_king] & pieces[WHITE][PAWN]) early -= 15;
            else early -= 30;
        }
        if(pop_leq_1(king_moves[black_king] & pieces[BLACK][PAWN])) {
            if(king_moves[black_king] & pieces[BLACK][PAWN]) early += 15;
            else early += 30;
        }

#ifdef _ATTACKERS_DEFENDERS
        uint16_t attackers[2] = { 0 }, defenders[2];
#endif

        //  --- I.g --- Count number of defenders

        #ifdef _ATTACKERS_DEFENDERS
        defenders[WHITE] = pop_count(pieces[WHITE][ALL] & attack_zones[KNIGHT][white_king]);
        defenders[BLACK] = pop_count(pieces[BLACK][ALL] & attack_zones[KNIGHT][black_king]);
        #endif

        //  --- I.h --- Late game bonus for passed pawns

        if(passed_pawns[WHITE]) {
            if(passed_pawns[BLACK] == 0) late += 25;
        } else {
            if(passed_pawns[BLACK]) late -= 25;
        }

                //  --- II.f --- Pawn placement

#ifdef _PAWN_PLACEMENT
    #ifdef _ATTACKERS_DEFENDERS
        attackers[WHITE] += 2 * pop_count(pieces[WHITE][PAWN] & attack_zones[PAWN][black_king]);
        attackers[BLACK] += 2 * pop_count(pieces[BLACK][PAWN] & attack_zones[PAWN][white_king]);
    #else
        #ifdef _ATTACK_ZONES
            early += _ATTACK_ZONES * (pop_count(pieces[WHITE][PAWN] & attack_zones[PAWN][black_king]) - pop_count(pieces[BLACK][PAWN] & attack_zones[PAWN][white_king]));
        #endif
    #endif
#endif

        int16_t cut_val = 0

#ifdef _ROOK_PLACEMENT
            + (2 * (_ROOK_OPEN_FILE + _ROOK_AIMED_AT_WEAK + _ROOK_ON_7TH))
#endif
#ifdef _BAD_BISHOP
            + _BAD_BISHOP
#endif
#ifdef _PIECE_OBSTRUCTING_PAWN
            + (6 * _PIECE_OBSTRUCTING_PAWN)
#endif
#ifdef _KNIGHT_OUTPOST
            + (2 * 18)
#endif
#ifdef _UNDEFENDED_MINOR
            + (4 * _UNDEFENDED_MINOR)
#endif
#ifdef _CONNECTED_ROOKS
            + _CONNECTED_ROOKS
#endif

            + ((int16_t)num_of_pieces * (0
#ifdef _KING_SAFETY
            + (3 * _NEAR_OPEN_FILE)
#endif
#ifdef _KING_BETWEEN_ROOKS
            + (2 * _KING_BETWEEN_ROOKS)
#endif
#ifdef _ATTACKERS_DEFENDERS
            + (_ATTACKERS_DEFENDERS * max(0, 9 - max(defenders[WHITE], defenders[BLACK])))
#endif
#ifdef _ATTACK_ZONES
            + 7 * _ATTACK_ZONES
#endif

#ifdef _KING_PRESSURE
            + 8 * _KING_PRESSURE
#endif
#ifdef _2MOVE_ATTACKS
            + 10 * _2MOVE_ATTACKS
#endif
            )) / 16;

        fine_evaluation += ((num_of_pieces * early) + ((16 - num_of_pieces) * late)) / 16;
        break;

        if(wtm) {
            if(fine_evaluation >= beta + cut_val) return fine_evaluation;
            else if(fine_evaluation < alpha - cut_val) break;
        } else {
            if(-fine_evaluation >= beta + cut_val) return -fine_evaluation;
            else if(-fine_evaluation < alpha - cut_val) break;
        }

        early = 0;
        late = 0;


        /*  +---------------------------------------------------+
            | Phase II                                          |
            +---------------------------------------------------+
            | a. File strength near king / castling spots       |
            | b. King between rooks penalty                     |
            | c-g. Rook/bishop/knight/pawn/queen placement      |
            +---------------------------------------------------+

        Remaining evaluation values:

         -- early:  +   3 * _NEAR_OPEN_FILE
                    +        _KING_BETWEEN_ROOKS
                    +   8  * _KING_PRESSURE
                    +   10 * _2MOVE_ATTACKS
                    +   _ATTACKERS_DEFENDERS * (9 - max(defenders))
                    +   7 * _ATTACK_ZONES
         -- late:  0
         -- all:        2  * (_ROOK_OPEN_FILE + _ROOK_AIMED_AT_WEAK + _ROOK_ON_7TH)
                    +        _BAD_BISHOP
                    +   2  * max of knight PST values
                    +   4  * _UNDEFENDED_MINOR
                    +        _CONNECTED_ROOKS

        */



        //  --- II.a --- File strength around king / castling spots (early: 36)

        //  Prior to castling, a penalty is given for weak/open files.  If a player no longer has castling rights, then
        //  the file strength is examined wherever the king is.  (Early game only.)

#ifdef _KING_SAFETY
        if(castling[WHITE]) {
            if(((adjacent_files[64] & (weak_pawns[WHITE] | open_files[WHITE])) != 0) && (adjacent_files[2] & (weak_pawns[WHITE] | open_files[WHITE])))
                early -= _NO_GOOD_CASTLE;   // A penalty for the pawn structure being messed up in both castling destinations
        } else {
            early -= (_NEAR_WEAK_FILE * pop_count(adjacent_files[white_king % 8] & weak_pawns[WHITE]) + _NEAR_OPEN_FILE * pop_count(adjacent_files[white_king % 8] & open_files[WHITE]));
        }

        if(castling[BLACK]) {
            if(((adjacent_files[64] & (weak_pawns[BLACK] | open_files[BLACK])) != 0) && (adjacent_files[2] & (weak_pawns[BLACK] | open_files[BLACK])))
                early += _NO_GOOD_CASTLE;   // A penalty for the pawn structure being messed up in both castling destinations
        } else {
            early += ((_NEAR_WEAK_FILE * pop_count(adjacent_files[black_king % 8] & weak_pawns[BLACK])) + (_NEAR_OPEN_FILE * pop_count(adjacent_files[black_king % 8] & open_files[BLACK])));
        }
#endif


        //  --- II.b --- King between rooks penalty (early: _KING_BETWEEN_ROOKS)

#ifdef _KING_BETWEEN_ROOKS
        if((pieces[WHITE][ROOK] != 0) && ((pieces[WHITE][KING] & interceding_squares[bitscan(pieces[WHITE][ROOK])][bitscan_reverse(pieces[WHITE][ROOK])]) != 0))
            early -= _KING_BETWEEN_ROOKS * (2 - (int16_t)(castling[WHITE] != 0));
        if((pieces[BLACK][ROOK] != 0) && ((pieces[BLACK][KING] & interceding_squares[bitscan(pieces[BLACK][ROOK])][bitscan_reverse(pieces[BLACK][ROOK])]) != 0))
            early += _KING_BETWEEN_ROOKS * (2 - (int16_t)(castling[BLACK] != 0));
#endif


        //  --- II.c --- Rook file placement (2*(_ROOK_OPEN_FILE + _ROOK_AIMED_AT_WEAK + _ROOK_ON_7TH))

    #ifdef _ROOK_PLACEMENT

        x = pieces[WHITE][ROOK];

        while(x) {
            i = bitscan(x);
            if(exp_2(i % 8) & (open_files[WHITE]))  {
                fine_evaluation += _ROOK_OPEN_FILE;                     // Rook on open file
                if(exp_2(i % 8) & (weak_pawns[BLACK]))
                    fine_evaluation += _ROOK_AIMED_AT_WEAK;             // Aimed at weak pawn
            }
            if((i / 8) == 6) fine_evaluation += _ROOK_ON_7TH;           // Rook on seventh rank

        #ifdef _PIECE_OBSTRUCTING_PAWN
            if(exp_2(i + 8) & pieces[BLACK][PAWN])
                fine_evaluation += _PIECE_OBSTRUCTING_PAWN;
        #endif


        #ifdef _ATTACK_ZONES
            if(exp_2(i) & attack_zones[ROOK][black_king]) {

                #ifdef _ATTACKERS_DEFENDERS
                    if(rook_moves[i][((rook_masks[i] & (pieces[WHITE][PAWN] | pieces[BLACK][PAWN])) * rook_magics[i]) >> BMS] & king_moves[black_king])
                        attackers[WHITE]++;
                #endif

                early += _ATTACK_ZONES;
            }
        #endif

            x &= x - 1;
        }

        x = pieces[BLACK][ROOK];
        while(x) {
            i = bitscan(x);
            if(exp_2(i % 8) & (open_files[BLACK])) {
                fine_evaluation -= _ROOK_OPEN_FILE;                     // Rook on open file
                if(exp_2(i % 8) & (weak_pawns[WHITE]))
                    fine_evaluation -= _ROOK_AIMED_AT_WEAK;             // Aimed at weak pawn
            }
            if((i / 8) == 1) fine_evaluation -= _ROOK_ON_7TH;           // Rook on seventh rank

        #ifdef _PIECE_OBSTRUCTING_PAWN
            if(exp_2(i - 8) & pieces[WHITE][PAWN])
                fine_evaluation -= _PIECE_OBSTRUCTING_PAWN;
        #endif

        #ifdef _ATTACK_ZONES
            if(exp_2(i) & attack_zones[ROOK][white_king]) {

                #ifdef _ATTACKERS_DEFENDERS
                    if(rook_moves[i][((rook_masks[i] & (pieces[WHITE][PAWN] | pieces[BLACK][PAWN])) * rook_magics[i]) >> BMS] & king_moves[white_king])
                        attackers[BLACK]++;
                #endif
                    early -= _ATTACK_ZONES;
            }
        #endif

            x &= x - 1;
        }

#endif


        //  --- II.d --- Bishop Placement (all: _BAD_BISHOPS)

#ifdef _BISHOP_PLACEMENT
        x = pieces[WHITE][BISHOP];
        while(x) {
            i = bitscan(x);
            y = pieces[WHITE][PAWN] | pieces[BLACK][PAWN];
            y = ((y & not_mask_file[0]) >> 9) | ((y & not_mask_file[7]) >> 7);

        #ifdef _BAD_BISHOP
            if((mask_latter_ranks[i] & empty & bishop_moves[i][((bishop_masks[i] & (pieces[WHITE][PAWN] | pieces[BLACK][PAWN])) * bishop_magics[i]) >> BMS] & ~y) == 0)
                fine_evaluation -= _BAD_BISHOP;
        #endif

        #ifdef _PIECE_OBSTRUCTING_PAWN
            if(exp_2(i + 8) & pieces[BLACK][PAWN])
                fine_evaluation += _PIECE_OBSTRUCTING_PAWN;
        #endif

        #ifdef _ATTACK_ZONES
            if(exp_2(i) & attack_zones[BISHOP][black_king]) {

                #ifdef _ATTACKERS_DEFENDERS
                    if(bishop_moves[i][((bishop_masks[i] & (pieces[WHITE][PAWN] | pieces[BLACK][PAWN])) * bishop_magics[i]) >> BMS] & king_moves[black_king])
                        attackers[WHITE]++;
                #endif
                    early += _ATTACK_ZONES;
            }
        #endif

            x &= x - 1;
        }

        x = pieces[BLACK][BISHOP];
        while(x) {
            i = bitscan(x);
            y = pieces[WHITE][PAWN] | pieces[BLACK][PAWN];
            y = ((y & not_mask_file[0]) << 7) | ((y & not_mask_file[7]) << 9);

        #ifdef _BAD_BISHOP
            if((mask_prior_ranks[i] & empty & bishop_moves[i][((bishop_masks[i] & (pieces[WHITE][PAWN] | pieces[BLACK][PAWN])) * bishop_magics[i]) >> BMS] & ~y) == 0)
                fine_evaluation += _BAD_BISHOP;
        #endif

        #ifdef _PIECE_OBSTRUCTING_PAWN
            if(exp_2(i - 8) & pieces[WHITE][PAWN])
                fine_evaluation -= _PIECE_OBSTRUCTING_PAWN;
        #endif

        #ifdef _ATTACK_ZONES
            if(exp_2(i) & attack_zones[BISHOP][white_king]) {

                #ifdef _ATTACKERS_DEFENDERS
                    if(bishop_moves[i][((bishop_masks[i] & (pieces[WHITE][PAWN] | pieces[BLACK][PAWN])) * bishop_magics[i]) >> BMS] & king_moves[white_king])
                        attackers[BLACK]++;
                #endif

                    early -= _ATTACK_ZONES;
            }
        #endif

            x &= x - 1;
        }
#endif


        //  --- II.e --- Knight placement (all: 2 * max knight pst vals)

#ifdef _KNIGHT_PLACEMENT
        x = pieces[WHITE][KNIGHT];
        while(x) {
            i = bitscan(x);

        #ifdef _PIECE_OBSTRUCTING_PAWN
            if(exp_2(i + 8) & pieces[BLACK][PAWN])
                fine_evaluation += _PIECE_OBSTRUCTING_PAWN;
        #endif

        #ifdef _KNIGHT_OUTPOST
            if((mask_latter_ranks[i + 8] & corresponding_files[adjacent_files[i % 8] ^ (i % 8)] & pieces[BLACK][PAWN]) == 0)
                fine_evaluation += max((int16_t)_KNIGHT_OUTPOST, PST[WHITE][KNIGHT][i]);
        #endif

        #ifdef _ATTACK_ZONES
            if(exp_2(i) & attack_zones[KNIGHT][black_king]) {

            #ifdef _ATTACKERS_DEFENDERS
                attackers[WHITE]++;
            #endif
                early += _ATTACK_ZONES;

            }
        #endif

            x &= x - 1;
        }

        x = pieces[BLACK][KNIGHT];
        while(x) {
            i = bitscan(x);

        #ifdef _PIECE_OBSTRUCTING_PAWN
            if(exp_2(i - 8) & pieces[WHITE][PAWN])
                fine_evaluation -= _PIECE_OBSTRUCTING_PAWN;
        #endif

        #ifdef _KNIGHT_OUTPOST
            if((mask_prior_ranks[i - 8] & corresponding_files[adjacent_files[i % 8] ^ (i % 8)] & pieces[WHITE][PAWN]) == 0)
                fine_evaluation -= max((int16_t)_KNIGHT_OUTPOST, PST[BLACK][KNIGHT][i]);
        #endif

        #ifdef _ATTACK_ZONES
            if(exp_2(i) & attack_zones[KNIGHT][white_king]) {

                #ifdef _ATTACKERS_DEFENDERS
                    attackers[BLACK]++;
                #endif
                    early -= _ATTACK_ZONES;

            }
        #endif

            x &= x - 1;
        }
#endif

        //  --- II.g --- Queen placement

#ifdef _QUEEN_PLACEMENT
        x = pieces[WHITE][QUEEN];
        while(x) {
            i = bitscan(x);

        #ifdef _ATTACK_ZONES
            if(exp_2(i) & (attack_zones[ROOK][black_king] | attack_zones[BISHOP][black_king])) {

            #ifdef _ATTACKERS_DEFENDERS
                if((bishop_moves[i][((bishop_masks[i] & (pieces[WHITE][PAWN] | pieces[BLACK][PAWN])) * bishop_magics[i]) >> BMS] & king_moves[black_king])
                    || (rook_moves[i][((rook_masks[i] & (pieces[WHITE][PAWN] | pieces[BLACK][PAWN])) * rook_magics[i]) >> RMS] & king_moves[black_king]))
                    attackers[WHITE]++;
            #endif

                early += _ATTACK_ZONES;

            }
        #endif

            x &= x - 1;
        }

        x = pieces[BLACK][QUEEN];
        while(x) {
            i = bitscan(x);

        #ifdef _ATTACK_ZONES
            if(exp_2(i) & (attack_zones[ROOK][white_king] | attack_zones[BISHOP][white_king])) {

            #ifdef _ATTACKERS_DEFENDERS
                if((bishop_moves[i][((bishop_masks[i] & (pieces[WHITE][PAWN] | pieces[BLACK][PAWN])) * bishop_magics[i]) >> BMS] & king_moves[white_king])
                    || (rook_moves[i][((rook_masks[i] & (pieces[WHITE][PAWN] | pieces[BLACK][PAWN])) * rook_magics[i]) >> RMS] & king_moves[white_king]))
                    attackers[BLACK]++;
            #endif

                early -= _ATTACK_ZONES;

            }
        #endif

            x &= x - 1;
        }
#endif


        //  --- II.h --- Attackers vs defenders

#ifdef _ATTACKERS_DEFENDERS
        early += max((int16_t)_ATTACKERS_DEFENDERS * (attackers[WHITE] - defenders[BLACK]), 0);
        early -= max((int16_t)_ATTACKERS_DEFENDERS * (attackers[BLACK] - defenders[WHITE]), 0);
#endif

        cut_val = 0
#ifdef _UNDEFENDED_MINOR
            + (4 * _UNDEFENDED_MINOR)
#endif
#ifdef _CONNECTED_ROOKS
            + _CONNECTED_ROOKS
#endif

            + ((int16_t)num_of_pieces * (0
#ifdef _KING_PRESSURE
            + 8*_KING_PRESSURE
#endif
#ifdef _2MOVE_ATTACKS
            + 10*_2MOVE_ATTACKS
#endif
            )) / 16;

        fine_evaluation += ((num_of_pieces * early) + ((16 - num_of_pieces) * late)) / 16;

        if(wtm) {
            if(fine_evaluation >= beta + cut_val) return fine_evaluation;
            else if(fine_evaluation < alpha - cut_val) break;
        } else {
            if(-fine_evaluation >= beta + cut_val) return -fine_evaluation;
            else if(-fine_evaluation < alpha - cut_val) break;
        }

        early = 0;
        late = 0;


        /*  +-----------------------------------+
            | Phase III                         |
            +-----------------------------------+
            | a. Undefended minor pieces        |
            | b. Connected rooks                |
            | c. King pressure                  |
            | d. Two-move attacks               |
            +-----------------------------------+

        Remaining evaluation value

        -- early:       8  * _KING_PRESSURE
                    +   10 * _2MOVE_ATTACKS
        -- late:        0
        -- all:         4 * _UNDEFENDED_MINOR
                    +       _CONNECTED_ROOKS

        */

        attack_set_gen(WHITE, SEE_bbs);
        attack_set_gen(BLACK, SEE_bbs);


        //  --- III.a --- Undefended minor pieces (all: 4 * _UNDEFENDED_MINOR)

#ifdef _UNDEFENDED_MINOR
        fine_evaluation -= _UNDEFENDED_MINOR * (pop_count((pieces[WHITE][BISHOP] | pieces[WHITE][KNIGHT]) & ~SEE_bbs[WHITE][ALL])
                                                - pop_count((pieces[BLACK][BISHOP] | pieces[BLACK][KNIGHT]) & ~SEE_bbs[BLACK][ALL]));
#endif

        //  --- III.b --- Connected rook bonus (all: _CONNECTED_ROOKS)

#ifdef _CONNECTED_ROOKS
        if(SEE_bbs[WHITE][ROOK] & pieces[WHITE][ROOK]) {
            if((SEE_bbs[BLACK][ROOK] & pieces[BLACK][ROOK]) == 0)
                fine_evaluation += _CONNECTED_ROOKS;
        } else {
            if(SEE_bbs[BLACK][ROOK] & pieces[BLACK][ROOK])
                fine_evaluation -= _CONNECTED_ROOKS;
        }
#endif


        //  --- III.c --- Counting threatened squares near king (early: 8 * _KING_PRESSURE)

#ifdef _KING_PRESSURE
        early -= _KING_PRESSURE * (pop_count(SEE_bbs[BLACK][ALL] & king_moves[white_king]) - pop_count(SEE_bbs[WHITE][ALL] & king_moves[black_king]));
#endif


        //  --- III.d --- Two move attacks (early: 10 * _2MOVE_ATTACKS)

#ifdef _2MOVE_ATTACKS
        int16_t                 ev_temp_score = 0;

        x = pieces[BLACK][QUEEN] | king_moves[black_king] | pieces[BLACK][KING];

        while(x) {
            i = bitscan(x);
            y = bishop_moves[i][((bishop_masks[i] & occupied) * bishop_magics[i]) >> BMS];
            if(knight_moves[i] & SEE_bbs[WHITE][KNIGHT] & ~SEE_bbs[BLACK][ALL]) ev_temp_score += _2MOVE_ATTACKS;
            if(y & SEE_bbs[WHITE][BISHOP] & ~SEE_bbs[BLACK][ALL]) ev_temp_score += _2MOVE_ATTACKS;
            x &= (x - 1);
        }

        x = pieces[WHITE][QUEEN] | king_moves[white_king] | pieces[WHITE][KING];

        while(x) {
            i = bitscan(x);
            y = bishop_moves[i][((bishop_masks[i] & occupied) * bishop_magics[i]) >> BMS];
            if(knight_moves[i] & SEE_bbs[BLACK][KNIGHT] & ~SEE_bbs[WHITE][ALL]) ev_temp_score -= _2MOVE_ATTACKS;
            if(y & SEE_bbs[BLACK][BISHOP] & ~SEE_bbs[WHITE][ALL]) ev_temp_score -= _2MOVE_ATTACKS;
            x &= (x - 1);
        }

        early += ev_temp_score / 2;
#endif

        fine_evaluation += ((num_of_pieces * early) + ((16 - num_of_pieces) * late)) / 16;

        if(wtm) return fine_evaluation;
        return -fine_evaluation;

    } while(false);

    attack_set_gen(btm, SEE_bbs);

    if(wtm) return fine_evaluation;
    return -fine_evaluation;


}

// ====================================================================================
//  This function evaluates the pawn structure and interacts with the pawn hash table.
// ====================================================================================

int16_t CGame::evaluate_pawn_structure() {

    if(probe_pawn_hash()) {
        open_files[0] = ht_open_files[0][hash_key_pawns()];
        open_files[1] = ht_open_files[1][hash_key_pawns()];
        weak_pawns[0] = ht_weak_pawns[0][hash_key_pawns()];
        weak_pawns[1] = ht_weak_pawns[1][hash_key_pawns()];
        passed_pawns[0] = ht_passed_pawns[0][hash_key_pawns()];
        passed_pawns[1] = ht_passed_pawns[1][hash_key_pawns()];
        return ht_value_pawns[hash_key_pawns()];
    }

    uint8_t i;
    int eval = 0;

    uint64_t white_pawns = pieces[1][1];
    uint64_t black_pawns = pieces[0][1];

    open_files[0] = 255;
    open_files[1] = 255;
    weak_pawns[0] = 0;
    weak_pawns[1] = 0;
    passed_pawns[0] = 0;
    passed_pawns[1] = 0;

#ifdef _PAWN_CHAIN
    eval += 4 * ((pop_count(pieces[WHITE][PAWN] & ((pieces[WHITE][PAWN] & ~maskfile(0)) << 7)) + pop_count(pieces[WHITE][PAWN] & ((pieces[WHITE][PAWN] & ~maskfile(7)) << 9)))
                - (pop_count(pieces[BLACK][PAWN] & ((pieces[BLACK][PAWN] & ~maskfile(0)) >> 9)) + pop_count(pieces[BLACK][PAWN] & ((pieces[BLACK][PAWN] & ~maskfile(7)) >> 7))));
#endif


    while(white_pawns) {
        i = bitscan(white_pawns);

        switch(i % 8) {

            case 0: // (Pawn is on H-file)

                open_files[WHITE] &= 254;

                if(pieces[WHITE][PAWN] & (uint64_t)0x0202020202020202) {

                    #ifdef _PAWN_BACKWARD   /* Backwards pawn */
                        if(((pieces[WHITE][PAWN] & (uint64_t)0x0202020202020202) & mask_prior_ranks[i]) == 0) {
                            weak_pawns[WHITE] |= 1;
                            eval -= _PAWN_BACKWARD;
                        }
                    #endif

                } else {

                    #ifdef _PAWN_ISOLATED   /* Isolated pawn */
                        weak_pawns[WHITE] |= 1;
                        eval -= _PAWN_ISOLATED;
                    #endif

                }

                    #ifdef _PAWN_PASSED     /* Passed pawn */
                        if((pieces[BLACK][PAWN] & ((uint64_t)0x0303030303030303) & mask_latter_ranks[i+8]) == 0) {
                            passed_pawns[WHITE] |= 1;
                            eval += _PAWN_PASSED * (1 + (i/8));
                        }
                    #endif

            break;




            case 7: // (Pawn is on A-file)

                open_files[WHITE] &= 127;

                if(pieces[WHITE][PAWN] & (uint64_t)0x4040404040404040) {

                    #ifdef _PAWN_BACKWARD   /* Backward pawn */
                        if(((pieces[WHITE][PAWN] & (uint64_t)0x4040404040404040) & mask_prior_ranks[i]) == 0) {
                            weak_pawns[WHITE] |= 128;
                            eval -= _PAWN_BACKWARD;
                        }
                    #endif

                } else {

                    #ifdef _PAWN_ISOLATED   /* Isolated pawn */
                        weak_pawns[WHITE] |= 128;
                        eval -= _PAWN_ISOLATED;
                    #endif

                }

                    #ifdef _PAWN_PASSED     /* Passed pawn */
                        if((pieces[BLACK][PAWN] & ((uint64_t)0xc0c0c0c0c0c0c0c0) & mask_latter_ranks[i+8]) == 0) {
                            passed_pawns[WHITE] |= 128;
                            eval += _PAWN_PASSED * (1 + (i/8));
                        }
                    #endif

            break;




            default: // (Pawn is on neither H- nor A- file)

                open_files[WHITE] &= ~exp_2(i % 8);

                if((pieces[WHITE][PAWN] & ((maskfile((i % 8) - 1) | maskfile((i % 8) + 1)) & mask_prior_ranks[i])) == 0) {

                    if(pieces[WHITE][PAWN] & ((maskfile((i % 8) - 1) | maskfile((i % 8) + 1)) & mask_latter_ranks[i])) {

                        #ifdef _PAWN_BACKWARD   /* Backward pawn */
                            weak_pawns[WHITE] |= exp_2(i % 8);
                            eval -= _PAWN_BACKWARD;
#endif

                    } else {

                        #ifdef _PAWN_ISOLATED   /* Isolated pawn */
                            weak_pawns[WHITE] |= exp_2(i % 8);
                            eval -= _PAWN_ISOLATED;
                        #endif
                    }
                }

                    #ifdef _PAWN_PASSED         /* Passed pawn */
                        if((pieces[BLACK][PAWN] & (maskfile((i%8)-1) | maskfile(i%8) | maskfile((i%8)+1)) & mask_latter_ranks[i+8]) == 0) {
                            passed_pawns[WHITE] |= exp_2(i % 8);
                            eval += _PAWN_PASSED * (1 + (i / 8));
                        }
                    #endif
            break;
        }

            #ifdef _PAWN_DOUBLED                /* Doubled, tripled, etc, pawns */
                if(pieces[WHITE][PAWN] & (maskfile(i%8) & ~exp_2(i))) {
                    weak_pawns[WHITE] |= exp_2(i % 8);
                    eval -= _PAWN_DOUBLED;
                }
            #endif

        white_pawns &= white_pawns - 1;
    }

    while(black_pawns){
        i = bitscan(black_pawns);

        switch(i % 8){
            case 0: // (Pawn is on H-file)

                open_files[BLACK] &= 254;

                if(pieces[BLACK][PAWN] & (uint64_t)0x0202020202020202) {

                    #ifdef _PAWN_BACKWARD   /* Backward pawn */
                        if(((pieces[BLACK][PAWN] & (uint64_t)0x0202020202020202) & mask_latter_ranks[i]) == 0)  {
                            weak_pawns[BLACK] |= exp_2(i % 8);
                            eval += _PAWN_BACKWARD;
                        }
                    #endif

                } else {

                    #ifdef _PAWN_ISOLATED   /* Isolated pawn */
                        weak_pawns[BLACK] |= exp_2(i % 8);
                        eval += _PAWN_ISOLATED;
                    #endif

                }

                #ifdef _PAWN_PASSED         /* Passed pawn */
                    if((pieces[WHITE][PAWN] & ((uint64_t)0x0303030303030303) & mask_prior_ranks[i-8]) == 0) {
                        passed_pawns[BLACK] |= 1;
                        eval -= _PAWN_PASSED * (8 - (i/8));
                    }
                #endif

            break;

            case 7: // (Pawn is on A-file)
                open_files[BLACK] &= 127;

                if(pieces[BLACK][PAWN] & (uint64_t)0x4040404040404040) {

                    #ifdef _PAWN_BACKWARD   /* Backward pawn */
                        if(((pieces[BLACK][PAWN] & (uint64_t)0x4040404040404040) & mask_latter_ranks[i]) == 0) {
                            weak_pawns[BLACK] |= 128;
                            eval += _PAWN_BACKWARD;
                        }
                    #endif

                } else {

                    #ifdef _PAWN_ISOLATED   /* Isolated pawn */
                        weak_pawns[BLACK] |= 128;
                        eval += _PAWN_ISOLATED;
                    #endif

                }

                #ifdef _PAWN_PASSED         /* Passed pawn */

                    // Note: if((mask_file[0] | mask_file[1]) != (uint64_t)0xc0c0c0c0c0c0c0c0) never triggers, yet,
                    // different behavior when I substitute one for the other?

                    if((pieces[WHITE][PAWN] & (mask_file[0] | mask_file[1]) /*(uint64_t)0xc0c0c0c0c0c0c0c0*/ & mask_prior_ranks[i-8]) == 0) {
                        passed_pawns[BLACK] |= 128;
                        eval -= _PAWN_PASSED * (8 - (i/8));
                    }
                #endif



            break;

            default: // (Pawn is on neither H- nor A- file)
                open_files[BLACK] &= ~exp_2(i % 8);

                if((pieces[BLACK][PAWN] & ((maskfile((i % 8) - 1) | maskfile((i % 8) + 1)) & mask_latter_ranks[i])) == 0) {

                    if(pieces[0][1] & ((mask_file_rv[(i % 8) - 1] | mask_file_rv[(i % 8) + 1]) & mask_prior_ranks[i])) {

                        #ifdef _PAWN_BACKWARD   /* Backward pawn */
                            weak_pawns[BLACK] |= exp_2(i % 8);
                            eval += _PAWN_BACKWARD;
                        #endif

                    } else {

                        #ifdef _PAWN_ISOLATED   /* Isolated pawn */
                            weak_pawns[BLACK] |= exp_2(i % 8);
                            eval += _PAWN_ISOLATED;
                        #endif

                    }
                }

                #ifdef _PAWN_PASSED             /* Passed pawn */
                    if((pieces[WHITE][PAWN] & (maskfile((i%8)-1) | maskfile(i%8) | maskfile((i%8)+1)) & mask_prior_ranks[i-8]) == 0) {
                        passed_pawns[BLACK] |= exp_2(i % 8);
                        eval -= _PAWN_PASSED * (8 - (i / 8));
                    }
                #endif

            break;
        }

        #ifdef _PAWN_DOUBLED                    /* Doubled, tripled, etc, pawn */
            if(pieces[BLACK][PAWN] & (maskfile(i%8) & ~exp_2(i))) {
                weak_pawns[BLACK] |= exp_2(i % 8);
                eval += _PAWN_DOUBLED;
            }
        #endif

        black_pawns &= black_pawns - 1;
    }

    new_pawn_hash(eval);
    return eval;
}


// ===================================================================================================================================
//  Is drawn only covers the cases of draw by fifty move rule and draw by threefold repitition.  (Stalemates are handled by movegen.)
// ===================================================================================================================================

bool CGame::is_drawn() {
    uint8_t fifty_move = hist_50m[move_num];
    if(fifty_move >= 100) return true;

    if((ht_threefold[zobrist_key >> (64 - 14)] >= 3) && (fifty_move >= 4)) {
        uint8_t x = 0;
        for(uint16_t i = move_num - fifty_move; i < move_num; i++) {
            if(hist_zobrist[i] == zobrist_key) x++;
        }
        if(x >= 2) return true;
    }

    return false;
}

bool CGame::repeated_or_50m(uint16_t ply) {
    uint8_t fifty_move = hist_50m[move_num];
    if(fifty_move >= 100) return true;

    if((ht_threefold[zobrist_key >> (64 - 14)] >= 2) && (fifty_move >= 2)) {
        uint8_t x = 0, y = 0, i;
        if(fifty_move > ply) {
            for(i = move_num - fifty_move; i < move_num - ply; i++) if(hist_zobrist[i] == zobrist_key)  x++;
            for(i = move_num - ply; i < move_num; i++)              if(hist_zobrist[i] == zobrist_key)  y++;
        } else {
            for(i = move_num - fifty_move; i < move_num; i++)       if(hist_zobrist[i] == zobrist_key)  y++;
        }
        if(y >= 1) return true;
        if(x+y >= 2) return true;
    }

    return false;
}

bool CGame::repeated(uint16_t ply) {
    if(ht_threefold[zobrist_key >> (64 - 14)] < 2)
        return false;
    for(uint16_t i = move_num - ply; i < move_num; i++)
        if(hist_zobrist[i] == zobrist_key) return true;
    return false;
}

// ==========================================================================================================================
//  Is promoting returns true if a the player to move has a pawn which is one rank away from promoting, and false otherwise.
// ==========================================================================================================================

bool CGame::is_promoting() {
    return (pieces[wtm][1] & mask_rank[promoting_rank[wtm]]) != 0;
}

// ===========================================================================================================
//  Have piece returns true if the player to move has a piece other than a king or pawn.  Used for null move.
// ===========================================================================================================

bool CGame::have_piece() {
    return (pieces[wtm][ALL] ^ (pieces[wtm][KING] | pieces[wtm][PAWN])) != 0;
}

// ========================================================================================================
//  Determines if a given move is a passed pawn move.  Requires pawn structure evaluation to be run first.
// ========================================================================================================

bool CGame::is_passed_pawn_move(uint16_t move) {
    uint8_t source = (uint8_t)extract_source(move), dest = (uint8_t)extract_dest(move);

    /*  Is piece moving a pawn? */

    if(board[source] != PAWN) return false;

    /*  Is piece moving on a passed pawn file?  */

    if((passed_pawns[wtm] & exp_2(source % 8)) == 0) return false;

    return true;
}

// ===============================
//  Fill the piece square tables.
// ===============================

void fill_PST(int16_t C[7][3]) {

    /*  In the early game, pawns are encouraged to occupy king-protecting or center-controlling squares.    */

    int16_t PST_pawn_zones[64]      = {  0,  0,  0,  0,  0,  0,  0,  0,
                                         2,  2,  2,  0,  0,  2,  2,  2,
                                         1,  1,  1,  5,  5,  5,  1,  1,
                                         -1,  -1,  6, 9, 9, 8,  -1,  -1,
                                         -1,  -1,  6, 9, 9, 8,  -1,  -1,
                                         1,  1,  1,  5,  5,  5,  1,  1,
                                         2,  2,  2,  0,  0,  2,  2,  2,
                                         0,  0,  0,  0,  0,  0,  0,  0 };

    int16_t PST_knight_zones[64]    = { -25, -20, -15, -10, -10, -15, -20, -25,     // The 4's were 0's
                                        -20, -15,  -8,  -8,  -8,  -8, -15, -20,
                                        -15,   4,  10,  15,  15,  10,   4, -15,
                                        -10,   5,  15,  18,  18,  15,   5, -10,
                                        -10,   5,  15,  18,  18,  15,   5, -10,
                                        -15,   4,  10,  15,  15,  10,   4, -15,
                                        -20, -15,  -8,  -8,  -8,  -8, -15, -20,
                                        -25, -20, -15, -10, -10, -15, -20, -25};

    int16_t PST_bishop_zones[64]    = { -10, -12, -14, -15, -15, -14, -13, -15,
                                          0,   5,  -5,  -3,  -3,  -5,   5,   0,
                                          5,   6,   5,   9,   9,   7,   6,   5,
                                          5,   8,   9,  10,  10,   9,   8,   5,
                                          5,   8,   9,  10,  10,   9,   8,   5,
                                          5,   6,   5,   9,   9,   7,   6,   5,
                                          0,   5,  -5,  -3,  -3,  -5,   5,   0,
                                        -10, -12, -14, -15, -15, -14, -13, -15};

    int16_t PST_pawn_ranks[8]       =  {  0,  1,  2,  5,  7,  20, 30, 0 };

    int16_t PST_pawn_cols[8]        =  { 1, 1, 4, 8, 8, 6, 1, 1 };

    //int16_t PST_knight_ranks[8]       =  { -1, 0, 1, 2, 3, 4, 3, 2 };
    //int16_t PST_knight_cols[8]        =  { -2, 0, 1, 2, 2, 1, 0, -2 };

    int16_t PST_knight_ranks[8] = { -2, 0, 1, 2, 2, 1, 0, -1 };
    int16_t PST_knight_cols[8] = { -2, 0, 1, 2, 2, 1, 0, -1 };


    int16_t PST_bishop_diag[15]     =  { -2, -1, 0, 1, 2, 4, 8, 9, 8, 4, 2, 1, 0, -1, -2 };

    // Note: (x, y) is on diagonal i if y = x - (7 - i) [for the NW-SE diags]
    //       (x, y) is on diagonal j if y = -x + i [for the SW-NE diags]

    int16_t king_dan_early[64]      = { 10, 20,  4,  0,  4, 20, 20, 10,
                                        8,  5,  2, -5,  2,  5,  5,  8,
                                        -5,-15,-25,-30,-30,-15, -15,-5,
                                        -20,-30,-40,-50,-50,-40,-30,-20,
                                        -20,-30,-40,-50,-50,-40,-30,-20,
                                        -5,-15,-25,-30,-30,-15, -15,-5,
                                        8,  5,  2, -5,  2,  5,  5,  8,
                                        10, 20,  4,  0,  4, 20, 20, 10 };

    int16_t king_dan_late[64]       = { 0,  0,  0,  5,  5,  0,  0,  0,
                                        5,  7,  8, 10, 10,  8,  7,  5,
                                        9, 10, 15, 20, 20, 15, 10,  9,
                                        10, 15, 20, 30, 30, 20, 15, 10,
                                        10, 15, 20, 30, 30, 20, 15, 10,
                                        9, 10, 15, 20, 20, 15, 10,  9,
                                        5,  7,  8, 10, 10,  8,  7,  5,
                                        0,  0,  0,  5,  5,  0,  0,  0 };

    /*  The king PST's are interpolated between my PST's (above) and the PST's of my best friend Andrew
        Backes's.  His newest chess program is called DirtyBit.     */

    int16_t king_anner_early[64]    = { 20,  30,  10,   0,   0,  10,  30,  20,
                                        20,  20,   0,   0,   0,   0,  20,  20,
                                        -10, -20, -20, -20, -20, -20, -20, -10,
                                        -20, -30, -30, -40, -40, -30, -30, -20,
                                        -30, -40, -40, -50, -50, -40, -40, -30,
                                        -30, -40, -40, -50, -50, -40, -40, -30,
                                        -30, -40, -40, -50, -50, -40, -40, -30,
                                        -30, -40, -40, -50, -50, -40, -40, -30 };

    int16_t king_anner_late[64]     = { -30,-18,-18,-18,-18,-18,-18,-30,
                                        -18,-18,  0,  0,  0,  0,-18,-18,
                                        -18,-6, 12, 18, 18, 12,-6,-18,
                                        -18,-6, 18, 24, 24, 18,-6,-18,
                                        -18,-6, 18, 24, 24, 18,-6,-18,
                                        -18,-6, 12, 18, 18, 12,-6,-18,
                                        -18,-12,-6,  0,  0,-6,-12,-18,
                                        -30,-24,-18,-12,-12,-18,-24,-30 };

    int16_t king_alt_early[64]      = { 15, 15, 0, 0, 0, 15, 15, 15,
                                        10, 10, 0, 0, 0, 10, 10, 10,
                                        -5, -5, -10, -15, -15, -10, -5, -5,
                                        -10, -10, -15, -25, -25, -15, -10, -10,
                                        -15, -15, -20, -30, -30, -20, -15, -15,
                                        -15, -25, -25, -35, -35, -25, -25, -15,
                                        -10, -20, -20, -20, -20, -20, -20, -10,
                                        -10, -15, -15, -15, -15, -15, -15, -10 };






    uint8_t i, j;

    for(i = 0; i < 8; i++) {
        for(j = 0; j < 8; j++) {

            /* Pawns */

            PST[WHITE][PAWN][8 * i + j] = C[PAWN][2] * PST_pawn_zones[8 * i + j] + C[PAWN][0] * PST_pawn_ranks[i] + C[PAWN][1] * PST_pawn_cols[j];

            /* Knights */

            PST[WHITE][KNIGHT][8 * i + j] = C[KNIGHT][0] * PST_knight_ranks[j] + C[KNIGHT][1] * PST_knight_cols[i] + C[KNIGHT][2] * PST_knight_zones[8 * i + j];
            //PST[WHITE][KNIGHT][8 * i + j] = (7 * C[KNIGHT][0] * PST_knight_ranks[i] + 10 * C[KNIGHT][1] * PST_knight_cols[j] + C[KNIGHT][2] * PST_knight_zones[8 * i + j]) / (C[KNIGHT][0] + C[KNIGHT][1] + C[KNIGHT][2]) + 1;

            /* Bishops */

            PST[WHITE][BISHOP][8 * i + j] = C[BISHOP][0] * PST_bishop_diag[7 + i - j] + C[BISHOP][1] * PST_bishop_diag[i + j] + C[BISHOP][2] * PST_bishop_zones[8 * i + j];

        }

    }

    for(j = PAWN; j <= BISHOP; j++)
    for(uint8_t k = 0; k < 8; k++)
    for(uint8_t l = 0; l < 8; l++)
        PST[BLACK][j][8 * k + l] = -PST[WHITE][j][8 * (7 - k) + l];


}