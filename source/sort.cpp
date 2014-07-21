// sort.cpp
// created: 11:30AM 1/10/14
// author: Daniel Sparks
// license: See text/license.txt
// purpose: The purposes of this cpp file are for sorting algorithms and move ordering.

#include "exacto.h"
#include "inlines.h"

// ==========================================================
//  General move ordering algorithm: assign values and sort.
// ==========================================================

void CEngine::order_moves(CGame * game, uint16_t * movelist, int16_t * sortlist, uint8_t ply) {
    /* This function assumes that the position is not checkmate or stalemate. */
    if(movelist[1] == 0) return;
    assign_values(game, movelist, sortlist, ply);
    sort_remaining(movelist, sortlist, 0);
}

void CEngine::order_caps(CGame * game, uint16_t * movelist, int16_t * sortlist, uint64_t SEE_bbs[2][7]) {
    if(movelist[0] == 0) return;
    assign_cap_values(game, movelist, sortlist, SEE_bbs);
    if(movelist[1] == 0) return;
    sort_remaining(movelist, sortlist, 0);
}

// ====================================================================================================================
//  Assigns values to each move in a given move list, so they can be sorted.  Returns the number of moves in the list.
// ====================================================================================================================

void CEngine::assign_values(CGame * game, uint16_t * movelist, int16_t * sortlist, uint8_t ply) {
    uint16_t        suggestion          = BOGUS_MOVE,
                    PV                  = BOGUS_MOVE;
    uint8_t source,
                    dest,
                    attacker,
                    defender,
                    i                   = 0,
                    enemy_king          = bitscan(game->pieces[game->btm][KING]);


    // Hash table suggestion
    if(hash.probe(game->zobrist_key, 0) >= HASH_ALPHA)
        suggestion = hash.sugg[game->zobrist_key % hash.size];

    if(hash.PV_lock[game->zobrist_key % PV_size] == game->zobrist_key)
        PV = hash.PV_move[game->zobrist_key % PV_size];

    for(; movelist[i]; i++) {
        source          =   (uint8_t)extract_source(movelist[i]);
        dest            =   (uint8_t)extract_dest(movelist[i]);
        attacker        =   game->board[source];
        defender        =   game->board[dest];

        sortlist[i]         =   0;

        if(attacker <= BISHOP)
            game->wtm ? (sortlist[i] += PST[WHITE][attacker][dest] - PST[WHITE][attacker][source]) : (sortlist[i] -= PST[BLACK][attacker][dest] - PST[BLACK][attacker][source]);

#ifdef _ATTACK_ZONES
        if(attacker <= ROOK) {

            if(((exp_2(dest) & attack_zones[attacker][enemy_king]) != 0) && ((exp_2(source) & attack_zones[attacker][enemy_king]) == 0))
                sortlist[i] += _ATTACK_ZONES;

        } else if(attacker == QUEEN) {
            if(((exp_2(dest) & (attack_zones[BISHOP][enemy_king] | attack_zones[ROOK][enemy_king])) != 0) && ((exp_2(source) & (attack_zones[BISHOP][enemy_king] | attack_zones[ROOK][enemy_king])) == 0))
                sortlist[i] += _ATTACK_ZONES;

        }
#endif



    /*  We always search the principal variation first.     */

        if(movelist[i] == PV)

            sortlist[i] = SORT_PV;

    /*  Next up is a general hash table suggestion.     */

        if(movelist[i] == suggestion)

            sortlist[i] = SORT_SUGGESTION;

    /*  Captures are sorted via piece square tables and static exchange evaluation.     */

        else if(defender) {

            if(extract_special(movelist[i]) == PROMOTE_QUEEN)
                sortlist[i] += QUEEN_VAL - PAWN_VAL;

            if(defender <= BISHOP)
                game->wtm ? (sortlist[i] -= PST[BLACK][defender][dest]) : (sortlist[i] += PST[WHITE][defender][dest]);


            sortlist[i] += game->SEE(source, dest);

            if(sortlist[i] > 0)         sortlist[i] += SORT_GOOD_CAP;
            else                        sortlist[i] += SORT_BAD_CAP;

    /*  Killer moves are rewarded here.     */

        } else if(movelist[i] == killer_move[0][ply])

            sortlist[i] = SORT_KILLER + killer_counter[0][ply];

        else if(movelist[i] == killer_move[1][ply])

            sortlist[i] = SORT_KILLER + killer_counter[1][ply];

        else if(extract_special(movelist[i]) == PROMOTE_QUEEN)

            sortlist[i] += QUEEN_VAL - PAWN_VAL;

    }

}

void CEngine::assign_cap_values(CGame * game, uint16_t * movelist, int16_t * sortlist, uint64_t SEE_bbs[2][7]) {
    uint8_t         source,
                    dest,
                    attacker,
                    defender,
                    enemy_king = bitscan(game->pieces[game->btm][KING]);;

    for(uint8_t i = 0; movelist[i]; i++) {
        source = (uint8_t)extract_source(movelist[i]);
        dest = (uint8_t)extract_dest(movelist[i]);
        attacker = game->board[source];
        defender = game->board[dest];

        sortlist[i] = 0;

        if(attacker <= BISHOP)
            game->wtm ? (sortlist[i] += PST[WHITE][attacker][dest] - PST[WHITE][attacker][source]) : (sortlist[i] -= PST[BLACK][attacker][dest] - PST[BLACK][attacker][source]);

        if(defender <= BISHOP)
            game->wtm ? (sortlist[i] -= PST[BLACK][defender][dest]) : (sortlist[i] += PST[WHITE][defender][dest]);

        if((SEE_bbs[game->btm][ALL] & exp_2(dest)) == 0)
            sortlist[i] += piece_values[WHITE][defender];
        else if(piece_values[defender] > piece_values[attacker]) {
            sortlist[i] = piece_values[WHITE][defender] - piece_values[WHITE][attacker];
        } else
            sortlist[i] += game->SEE(source, dest);

        sortlist[i] += (sortlist[i] >= 0)? SORT_GOOD_CAP : SORT_BAD_CAP;
    }

}

// ===================================================================================
//  Insertion sorts the moves in depth "depth," beginning with the leftmost-th entry.
// ===================================================================================

void CEngine::sort_remaining(uint16_t * movelist, int16_t * sortlist, uint8_t leftmost) {
    int16_t         temp_val;
    uint16_t        temp_move;
    uint8_t i = leftmost + 1,
                    j;

    for(; movelist[i]; i++) {
        temp_move = movelist[i];
        temp_val = sortlist[i];
        for(j = i; j > leftmost; j--) {
            if(temp_val > sortlist[j-1]) {
                sortlist[j] = sortlist[j-1];
                movelist[j] = movelist[j-1];
            } else
                break;
        }
        sortlist[j] = temp_val;
        movelist[j] = temp_move;
    }
}

// ====================================================================
//  For killer move heuristics, this function is called during search.
// ====================================================================

void CEngine::update_killers(uint8_t depth, uint16_t move) {
    if(move == BOGUS_MOVE) return;

    killer_move[0][depth] = killer_move[1][depth];
    killer_move[1][depth] = move;

    if(killer_move[1][depth] == move) killer_counter[1][depth]++;
    else if(killer_move[0][depth] == move) killer_counter[0][depth]++;
    else {
        bool high = (killer_counter[1][depth] >= killer_counter[0][depth]);
        killer_move[!high][depth] = move;
        killer_counter[!high][depth] = 1;
    }
}