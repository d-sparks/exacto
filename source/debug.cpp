// debug.cpp
// created: 5:53PM 1/30/14
// author: Daniel Sparks
// license: See text/license.txt
// purpose: Debugging functions which aren't included with releases.

#include "exacto.h"
#include "inlines.h"

// ==========================================================================
//  Prints unsigned integers (64bits and 8 bits, respectively) through cout.
// ==========================================================================

void print_bitboard(uint64_t bb) {
    for(int i = 0; i <= 7; i++){
        for(int j = 0; j <= 7; j++){
            if(bb & two_to_the[(8*(7-i)) + (7-j)])
                cout << "1 ";
            else
                cout << "0 ";
            }
        cout << endl;
    }
    cout << endl;
}

// =====================================================
//  Prints all of the gamestate bitboards side by side.
// =====================================================

void CGame::print_all_bitboards() {
    for(int color = 1; color >= 0; color--){
        if (color == 1){
            cout << "White\n" << "Pawns,   Knights, Bishops, Rooks,   Queens,  King\n";
        } else {
            cout << "Black\n" << "Pawns,   Knights, Bishops, Rooks,   Queens,  King\n";
        }
        for(int i = 0; i <= 7; i++){
            for(int j = 1; j <= 6; j++){
                for(int k = 0; k <= 7; k++)
                    cout << ((pieces[color][j] & two_to_the[(8*(7-i)) + (7-k)]) != 0);
                cout << " ";
            }
            cout << "\n";
        }
        cout << "\n";
    }
}

// ========================================
//  Print move history for usermove input.
// ========================================

void CGame::print_move_history_i() {
    for(int i = 1; i <= move_num; i++){
        cout << "usermove ";
        print_move(hist_moves[i]);
        cout << " ";
    }
    cout << endl;
}

// =================================================
//  For comparing different versions' search trees.
// =================================================

    /* DEBUG DEBUG DEBUG */
/*if(depth == 8) {
    cout << endl;
    print_move(movelist[i]);
}*/

    /* DEBUG DEBUG DEBUG */
/*if((depth + ply == 8) && (ply < 3) && (ext_plies == 0)) {
    if(ply == 1) {
        cout << endl << "\t";           print_move(movelist[i]); cout << "\t";
    } else if(ply == 2) {
        print_move(movelist[i]); cout << " ";
    }

}*/

// ==============================
//  Search corruption detection.
// ==============================

// Use:

//  uint64_t local[2][7];           // Debug code
//  game->store_pieces(local);

// ...

//  if(game->is_corrupt(local)) { cout << "Locator:" << endl; game->freeze(movelist[i], local); }

bool CGame::is_corrupt(uint64_t the_pieces[2][7]) {
    for(int i = 0; i<= 1; i++)
        for(int j = 1; j <= 6; j++)
            if(the_pieces[i][j] != pieces[i][j])
                return true;
    return false;
}

void CGame::store_pieces(uint64_t the_pieces[2][7]) {
    for(int i = 0; i <= 1; i++)
        for(int j = 0; j <= 6; j++)
            the_pieces[i][j] = pieces[i][j];
}

void CGame::freeze(uint16_t move, uint64_t local_pieces[2][7]) {
    print_board();
    print_move_history_i();
    cout << endl; print_move(move); cout << " defender: " << (int)hist_defender[move_num] << endl;
    print_move(hist_moves[move_num + 2]); cout << " (possibly)" << endl;
    cout << "REAL:" << endl;
    print_all_bitboards();
    cout << endl << "SHOULD BE:" << endl;
    for(int color = 1; color >= 0; color--){
        if (color == 1){
            cout << "White\n" << "Pawns,   Knights, Bishops, Rooks,   Queens,  King\n";
        } else {
            cout << "Black\n" << "Pawns,   Knights, Bishops, Rooks,   Queens,  King\n";
        }
        for(int i = 0; i <= 7; i++){
            for(int j = 1; j <= 6; j++){
                for(int k = 0; k <= 7; k++){
                    if(local_pieces[color][j] & two_to_the[(8*(7-i)) + (7-k)]) {
                        cout << "1";
                    } else {
                        cout << "0";
                    }
                }
                cout << " ";
            }
            cout << "\n";
        }
        cout << "\n";
    }

    int k;
    cin >> k;
    if(k == 5) return;


}

// =======================================================================================
//  For printing out the evaluation of a position, to understand the evaluation function.
// =======================================================================================

void CGame::print_evaluate() {

}


// ==============================================
//  Create a file with many necessary bitboards.
// ==============================================

void make_bitboard_file(){
    ofstream text;
    text.open("bitboards_out.txt");

    // ---------------
    //  bitboards.cpp
    // ---------------

    text << endl << endl << "// bitboards.cpp" << endl << endl;

    uint64_t _exp_2[64] = { 1 }, x[64] = { 0 }, y[64] = { 0 }, z[64] = { 0 };
    int i, j, k, l;
    for(i = 1; i < 64; i++) _exp_2[i] = _exp_2[i - 1] << 1;

    // two_to_the

    text << "uint64_t\tconst\ttwo_to_the[64]\t\t\t\t=\t{\t";
    for(i = 0; i < 64; i++) {
        text << "0x" << setfill('0') << setw(16) << hex << _exp_2[i];
        if(i != 63) text << ",\t";
        if((i % 6) == 3) {
            if(i != 63) text << endl << "\t\t\t\t";
            else text << "};" << endl;
        }
    }

    // not_two_to_the
    text << "uint64_t\tconst\tnot_two_to_the[64]\t\t\t=\t{\t";
    for(i = 0; i < 64; i++) {
        text << "0x" << setfill('0') << setw(16) << hex << (~_exp_2[i]);
        if(i != 63) text << ",\t";
        if((i % 6) == 3) {
            if(i != 63) text << endl << "\t\t\t\t";
            else text << "};" << endl;
        }
    }

    // all set
    text << "uint64_t   const   all_set                     =       0xffffffffffffffff;" << endl;

    // mask ranks
    for(i = 0; i < 7; i++) {
        x[i] = _exp_2[8 * (i + 1)] - _exp_2[8 * i];
        x[i + 8] = ~x[i];
    }
    x[7] = 0xffffffffffffffff - (_exp_2[8 * 7] - 1);
    x[15] = ~x[7];
    text << "uint64_t\tconst\tmask_rank[8]\t\t\t\t=\t{\t";
    for(i = 0; i < 8; i++) {
        text << "0x" << setfill('0') << setw(16) << hex << x[i];
        if(i != 7) text << ",\t";
        if((i % 6) == 3) {
            if(i != 7) text << endl << "\t\t\t\t";
        }
    }
    text << "};" << endl;
    text << "uint64_t\tconst\tnot_mask_rank[8]\t\t\t=\t{\t";
    for(i = 8; i < 16; i++) {
        text << "0x" << setfill('0') << setw(16) << hex << x[i];
        if(i != 15) text << ",\t";
        if((i % 6) == 5) {
            if(i != 15) text << endl << "\t\t\t\t";
        }
    }
    text << "};" << endl;

    // mask prior and latter ranks
    for(i = 0; i <= 63; i++){
        y[i] = 0;
        z[i] = 0;
        for(j = 0; j <= ((i - (i % 8)) / 8); j++) y[i] |= x[j];
        for(j = ((i - (i % 8)) / 8); j <= 7; j++) z[i] |= x[j];
    }

    text << "uint64_t\tconst\tmask_prior_ranks[64]\t\t=\t{\t";
    for(i = 0; i < 64; i++) {
        text << "0x" << setfill('0') << setw(16) << hex << y[i];
        if(i != 63) text << ",\t";
        if((i % 6) == 3) {
            if(i != 63) text << endl << "\t\t\t\t";
            else text << "};" << endl;
        }
    }
    text << "uint64_t\tconst\tmask_latter_ranks[64]\t\t=\t{\t";
    for(i = 0; i < 64; i++) {
        text << "0x" << setfill('0') << setw(16) << hex << z[i];
        if(i != 63) text << ",\t";
        if((i % 6) == 3) {
            if(i != 63) text << endl << "\t\t\t\t";
            else text << "};" << endl;
        }
    }

    // mask files
    for(i = 0; i < 8; i++) {
        x[i] = 0;
        for(j = 0; j < 8; j++) x[i] |= _exp_2[i + 8 * j];
    }
    text << "uint64_t\tconst\tmask_file[8]\t\t\t\t=\t{\t";
    for(i = 0; i < 8; i++) {
        text << "0x" << setfill('0') << setw(16) << hex << x[7 - i];
        if(i != 7) text << ",\t";
        if((i % 6) == 3) {
            if(i != 7) text << endl << "\t\t\t\t";
        }
    }
    text << "};" << endl;
    text << "uint64_t\tconst\tmask_file_rv[8]\t\t\t\t=\t{\t";
    for(i = 0; i < 8; i++) {
        text << "0x" << setfill('0') << setw(16) << hex << x[i];
        if(i != 7) text << ",\t";
        if((i % 6) == 3) {
            if(i != 7) text << endl << "\t\t\t\t";
        }
    }
    text << "};" << endl;
    text << "uint64_t\tconst\tnot_mask_file[8]\t\t\t=\t{\t";
    for(i = 0; i < 8; i++) {
        text << "0x" << setfill('0') << setw(16) << hex << (~x[7 - i]);
        if(i != 7) text << ",\t";
        if((i % 6) == 3) {
            if(i != 7) text << endl << "\t\t\t\t";
        }
    }
    text << "};" << endl;

    // knight moves
    for(i = 0; i <= 7; i++){
        for(j = 0; j <= 7; j++){
            x[(8 * j) + i] = 0;
            if((i <= 6) & (j <= 5)) x[(8 * j) + i] += _exp_2[(8 * (j + 2)) + (i + 1)];     // Up two, left one
            if((i >= 1) & (j <= 5)) x[(8 * j) + i] += _exp_2[(8 * (j + 2)) + (i - 1)];     // Up two, right one
            if((i >= 2) & (j <= 6)) x[(8 * j) + i] += _exp_2[(8 * (j + 1)) + (i - 2)];     // Up one, right two
            if((i >= 2) & (j >= 1)) x[(8 * j) + i] += _exp_2[(8 * (j - 1)) + (i - 2)];     // Down one, right two
            if((i >= 1) & (j >= 2)) x[(8 * j) + i] += _exp_2[(8 * (j - 2)) + (i - 1)];     // Down two, right one
            if((i <= 6) & (j >= 2)) x[(8 * j) + i] += _exp_2[(8 * (j - 2)) + (i + 1)];     // Down two, left one
            if((i <= 5) & (j >= 1)) x[(8 * j) + i] += _exp_2[(8 * (j - 1)) + (i + 2)];     // Down one, left two
            if((i <= 5) & (j <= 6)) x[(8 * j) + i] += _exp_2[(8 * (j + 1)) + (i + 2)];     // Up one, left two
        }
    }
    text << "uint64_t\tconst\tknight_moves[64]\t\t\t=\t{\t";
    for(i = 0; i < 64; i++) {
        text << "0x" << setfill('0') << setw(16) << hex << x[i];
        if(i != 63) text << ",\t";
        if((i % 6) == 3) {
            if(i != 63) text << endl << "\t\t\t\t";
            else text << "};" << endl;
        }
    }

    // king moves
    for(i = 0; i <= 7; i++){
        for(j = 0; j <= 7; j++){
            x[(8 * j) + i] = 0;
            if(j <= 6) x[(8 * j) + i] += _exp_2[(8 * (j + 1)) + i];                         // Up
            if((i >= 1) & (j <= 6)) x[(8 * j) + i] += _exp_2[(8 * (j + 1)) + (i - 1)];      // Up, right
            if(i >= 1) x[(8 * j) + i] += _exp_2[(8 * j) + (i - 1)];                         // Right
            if((i >= 1) & (j >= 1)) x[(8 * j) + i] += _exp_2[(8 * (j - 1)) + (i - 1)];      // Down, right
            if(j >= 1) x[(8 * j) + i] += _exp_2[(8 * (j - 1)) + i];                         // Down
            if((i <= 6) & (j >= 1)) x[(8 * j) + i] += _exp_2[(8 * (j - 1)) + (i + 1)];      // Down, left
            if(i <= 6) x[(8 * j) + i] += _exp_2[(8 * j) + (i + 1)];                         // Left
            if((i <= 6) & (j <= 6)) x[(8 * j) + i] += _exp_2[(8 * (j + 1)) + (i + 1)];      // Up, left

        }
    }
    text << "uint64_t\tconst\tking_moves[64]\t\t\t\t=\t{\t";
    for(i = 0; i < 64; i++) {
        text << "0x" << setfill('0') << setw(16) << hex << x[i];
        if(i != 63) text << ",\t";
        if((i % 6) == 3) {
            if(i != 63) text << endl << "\t\t\t\t";
            else text << "};" << endl;
        }
    }

    for(i = 0; i <= 7; i++){
        for(j = 0; j <= 7; j++){
            x[(8 * j) + i] = 0;
            for(k = 1; k <= 6; k++){
                if(k != i) x[(8 * j) + i] += _exp_2[(8 * j) + k];  // Horizontal
                if(k != j) x[(8 * j) + i] += _exp_2[(8 * k) + i];  // Vertical
            }
            y[(8 * j) + i] = 0;
            for(k = i + 1, l = j + 1; max(k, l) <= 6; k++, l++) y[(8 * j) + i] += two_to_the[(8 * l) + k];       // Up, left
            for(k = i + 1, l = j - 1; (k <= 6) & (l >= 1); k++, l--) y[(8 * j) + i] += two_to_the[(8 * l) + k];  // Down, left
            for(k = i - 1, l = j + 1; (k >= 1) & (l <= 6); k--, l++) y[(8 * j) + i] += two_to_the[(8 * l) + k];  // Up, right
            for(k = i - 1, l = j - 1; min(k, l) >= 1; k--, l--) y[(8 * j) + i] += two_to_the[(8 * l) + k];       // Down, right
        }
    }

    text << "uint64_t\tconst\tbishop_masks[64]\t\t\t=\t{\t";
    for(i = 0; i < 64; i++) {
        text << "0x" << setfill('0') << setw(16) << hex << y[i];
        if(i != 63) text << ",\t";
        if((i % 6) == 3) {
            if(i != 63) text << endl << "\t\t\t\t";
            else text << "};" << endl;
        }
    }
    text << "uint64_t\tconst\trook_masks[64]\t\t\t\t=\t{\t";
    for(i = 0; i < 64; i++) {
        text << "0x" << setfill('0') << setw(16) << hex << x[i];
        if(i != 63) text << ",\t";
        if((i % 6) == 3) {
            if(i != 63) text << endl << "\t\t\t\t";
            else text << "};" << endl;
        }
    }

    // light dark squares
    text << "uint64_t\tconst\tlight_dark_squares[2]\t\t=\t{\t0x55aa55aa55aa55aa,\t0xaa55aa55aa55aa55\t};" << endl;
    text << "uint64_t\tconst\tmask_ep_ranks[2]\t\t\t=\t{\t0x" << setfill('0') << setw(16) << hex << 4278190080;
    text << ",\t0x" << setfill('0') << setw(16) << hex << 1095216660480 << "\t};" << endl;
    text << "uint8_t\t\tconst\tpromoting_rank[2]\t\t\t=\t{\t0x" << setfill('0') << setw(4) << hex << 1;
    text << ",\t\t\t\t0x" << setfill('0') << setw(4) << hex << 6 << "\t};" << endl;

    text << "uint8_t\t\tconst\tadjacent_files[8]\t\t\t=\t{\t";
    text << "0x" << setfill('0') << setw(4) << hex << 3 << ",\t\t\t\t";
    text << "0x" << setfill('0') << setw(4) << hex << 7 << ",\t\t\t\t";
    text << "0x" << setfill('0') << setw(4) << hex << 14 << ",\t\t\t\t";
    text << "0x" << setfill('0') << setw(4) << hex << 28 << "," << endl << "\t\t\t\t";
    text << "0x" << setfill('0') << setw(4) << hex << 56 << ",\t\t\t\t";
    text << "0x" << setfill('0') << setw(4) << hex << 112 << ",\t\t\t\t";
    text << "0x" << setfill('0') << setw(4) << hex << 224 << ",\t\t\t\t";
    text << "0x" << setfill('0') << setw(4) << hex << 192 << "\t};";




    // ----------
    //  hash.cpp
    // ----------

    text << endl << endl << "// hash.cpp" << endl << endl;
    text << "uint64_t\tconst\tzobrist_wtm\t\t\t\t\t=\t\t0x" << hex << zobrist_wtm << ";";
    text << endl;
    text << "uint64_t\tconst\tzobrist_castling[4]\t\t\t=\t{\t";
    for(int i = 0; i <= 3; i++) {
        text << "0x" << hex << zobrist_en_passant[i];
        if(i == 3) text << "};";
        else text << ",\t";
    }

    text << endl;
    text << "uint64_t\tconst\tzobrist_en_passant[8]\t\t=\t{\t";
    for(int i = 0; i <= 7; i++) {
        text << "0x" << hex << zobrist_en_passant[i];
        if(i == 7) text << "};";
        else {
            if(i == 3) text << "," << endl << "\t\t\t\t";
            else {
                text << ",\t";
            }
        }
    }
    text << endl;
    text << "uint64_t\tconst\tzobrist_pieces[2][7][64]\t=\t{{{\t";
    for(int i = 0; i <= 1; i++) {
        for(int j = 0; j <= 6; j++) {
            for(int k = 0; k <= 63; k++) {
                if(k != 0) text << "\t";
                text << "0x" << hex << zobrist_pieces[i][j][k];
                if(k != 63) text << ",";
                else {
                    text << "}";
                    if(j == 6) {
                        text << "}";
                        if(i == 1) text << "};";
                        else text << ",";
                    } else
                        text << ",";
                }
                if(k % 6 == 3) {
                    text << endl;
                    if(k != 63) text << "\t\t\t";
                    else {
                        if(j < 6) text << "\t\t\t\t\t\t\t\t\t\t\t\t\t{\t";
                        else {
                            if(i == 0) text << "\t\t\t\t\t\t\t\t\t\t\t\t\t{{\t";
                        }
                    }
                }
            }
        }
    }


    // ------------
    //  magics.cpp
    // ------------

    text << endl << endl << "// magics.cpp" << endl << endl;
    text << "uint64_t\tconst\tbishop_magics[64]\t\t\t=\t{\t";
    for(i = 0; i < 64; i++) {
        text << "0x" << setfill('0') << setw(16) << hex << bishop_magics[i];
        if(i != 63) text << ",\t";
        if((i % 6) == 3) {
            if(i != 63) text << endl << "\t\t\t\t";
            else text << "};" << endl;
        }
    }
    text << "uint64_t\tconst\trook_magics[64]\t\t\t\t=\t{\t";
    for(i = 0; i < 64; i++) {
        text << "0x" << setfill('0') << setw(16) << hex << rook_magics[i];
        if(i != 63) text << ",\t";
        if((i % 6) == 3) {
            if(i != 63) text << endl << "\t\t\t\t";
            else text << "};" << endl;
        }
    }

    text.close();

}

// =========================================================================================================
//  Theoretically, after the first make_bitboard_file is run, one can use those bitboards to make the rest.
// =========================================================================================================

void make_bitboard_file_2() {

    ofstream text;
    text.open("bitboards_out.txt");

    // ---------------
    //  bitboards.cpp
    // ---------------

    // attack zones

    uint64_t x[64] = { 0 }, y[64] = { 0 }, z[64] = { 0 }, w[64] = { 0 }, bb, bb2;
    uint8_t i, j;

    for(i = 0; i < 64; i++) {
        bb = king_moves[i] | two_to_the[i];
        bb2 = bb;

        y[i] = bb;

        while(bb) {
            j = bitscan(bb);
            bb &= bb - 1;

            // pawns
            x[i] = bb2 | (bb2 << 8) | (bb2 >> 8);

            // knights
            y[i] |= knight_moves[j];

            // bishops
            /* if((i % 8 == j % 8) || (i/8 == j/8)) */ z[i] |= bishop_moves[j][ALL];

            // rooks
            w[i] |= rook_moves[j][ALL];

        }
        bb = knight_moves[i];
        while(bb) {
            j = bitscan(bb);
            bb &= bb - 1;

            // knight double moves (should experiment with and without)
            y[i] |= knight_moves[j];
        }
    }

    text << endl << endl << "// bitboards.cpp" << endl << endl;

    text << "uint64_t\tconst\tpawn_attacks[64]\t\t\t=\t{\t";
    for(i = 0; i < 64; i++) {
        text << "0x" << setfill('0') << setw(16) << hex << x[i];
        if(i != 63) text << ",\t";
        if((i % 6) == 3) {
            if(i != 63) text << endl << "\t\t\t\t";
            else text << "};" << endl;
        }
    }
    text << "uint64_t\tconst\tknight_attacks[64]\t\t\t=\t{\t";
    for(i = 0; i < 64; i++) {
        text << "0x" << setfill('0') << setw(16) << hex << y[i];
        if(i != 63) text << ",\t";
        if((i % 6) == 3) {
            if(i != 63) text << endl << "\t\t\t\t";
            else text << "};" << endl;
        }
    }
    text << "uint64_t\tconst\tbishop_attacks[64]\t\t\t=\t{\t";
    for(i = 0; i < 64; i++) {
        text << "0x" << setfill('0') << setw(16) << hex << z[i];
        if(i != 63) text << ",\t";
        if((i % 6) == 3) {
            if(i != 63) text << endl << "\t\t\t\t";
            else text << "};" << endl;
        }
    }
    text << "uint64_t\tconst\trook_attacks[64]\t\t\t=\t{\t";
    for(i = 0; i < 64; i++) {
        text << "0x" << setfill('0') << setw(16) << hex << w[i];
        if(i != 63) text << ",\t";
        if((i % 6) == 3) {
            if(i != 63) text << endl << "\t\t\t\t";
            else text << "};" << endl;
        }
    }





    text.close();

}