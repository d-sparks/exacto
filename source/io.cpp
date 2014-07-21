// io.cpp
// created: 1:11 AM 12/10/13
// author: Daniel Sparks
// license: See text/license.txt
// purpose: Chess related functions not related to search or evaluations.

#include "exacto.h"
#include "inlines.h"

/* ===============================================================================
    This function prints a board to the console/prompt, using letters for pieces.
   =============================================================================== */

void CGame::print_board() {
    int local_board[64];
    int i, j, k;
    uint64_t x;

    for(i = 0; i <= 63; i++) local_board[i] = 0;

    x = pieces[WHITE][PAWN];    while(x) { i = bitscan(x); local_board[i] = 1; x = x & (~two_to_the[i]);}
    x = pieces[WHITE][KNIGHT];  while(x) { i = bitscan(x); local_board[i] = 2; x = x & (~two_to_the[i]);}
    x = pieces[WHITE][BISHOP];  while(x) { i = bitscan(x); local_board[i] = 3; x = x & (~two_to_the[i]);}
    x = pieces[WHITE][ROOK];    while(x) { i = bitscan(x); local_board[i] = 4; x = x & (~two_to_the[i]);}
    x = pieces[WHITE][QUEEN];   while(x) { i = bitscan(x); local_board[i] = 5; x = x & (~two_to_the[i]);}
    x = pieces[WHITE][KING];    while(x) { i = bitscan(x); local_board[i] = 6; x = x & (~two_to_the[i]);}
    x = pieces[BLACK][PAWN];    while(x) { i = bitscan(x); local_board[i] = -1; x = x & (~two_to_the[i]);}
    x = pieces[BLACK][KNIGHT];  while(x) { i = bitscan(x); local_board[i] = -2; x = x & (~two_to_the[i]);}
    x = pieces[BLACK][BISHOP];  while(x) { i = bitscan(x); local_board[i] = -3; x = x & (~two_to_the[i]);}
    x = pieces[BLACK][ROOK];    while(x) { i = bitscan(x); local_board[i] = -4; x = x & (~two_to_the[i]);}
    x = pieces[BLACK][QUEEN];   while(x) { i = bitscan(x); local_board[i] = -5; x = x & (~two_to_the[i]);}
    x = pieces[BLACK][KING];    while(x) { i = bitscan(x); local_board[i] = -6; x = x & (~two_to_the[i]);}

    if(en_passant) local_board[bitscan(en_passant)] = 7;

    for(i = 0; i <= 7; i++){
        cout << "\n     +---+---+---+---+---+---+---+---+\n  " << (int)(8-i) << "  ";
        for(j = 0; j <= 7; j++){
            k = (((7-i)*8) + (7-j));
            if(local_board[k] == 6) {
                cout << "|[K]";
            } else if(local_board[k] == -6) {
                cout << "|[k]";
            } else {
                if(local_board[k] == 0) cout << "|   ";
                if(local_board[k] == 1) cout << "| P ";
                if(local_board[k] == 2) cout << "| N ";
                if(local_board[k] == 3) cout << "| B ";
                if(local_board[k] == 4) cout << "| R ";
                if(local_board[k] == 5) cout << "| Q ";
                if(local_board[k] == -1) cout << "| p ";
                if(local_board[k] == -2) cout << "| n ";
                if(local_board[k] == -3) cout << "| b ";
                if(local_board[k] == -4) cout << "| r ";
                if(local_board[k] == -5) cout << "| q ";
                if(local_board[k] == 7) cout << "| : ";
            }
        }
        switch(i) {
            case 0:
                cout << "|     " << (wtm? "White to move" : "Black to move");
                break;
            case 1:
                cout << "|     Static evaluation: " << (wtm? evaluate(-MATESCORE, MATESCORE) : -evaluate(-MATESCORE, MATESCORE));
                break;
            case 2:
                //check[0] = in_check();
                cout << "|     Q-search value: "; // << (wtm? qsearch(-MATESCORE, MATESCORE, 0) : -qsearch(-MATESCORE, MATESCORE, 0));
                break;
            case 3:
/*              if((ht_threefold[zobrist_key >> (64 - 14)] >= 1) && (history_fifty_move[move_num] >= 4)) {
                    for(short i = move_num - history_fifty_move[move_num]; i < move_num; i++) {
                        if(history_zobrist[i] == zobrist_key) z++;
                    }
                }*/
                cout << "|     Repititions: ";// << (int)(z);
                break;
            case 4:
                cout << "|     Castling     Kingside/Queenside";
                break;
            case 5:
                cout << "|     White:          " << (((castling[1] & two_to_the[1]) != 0)? "Yes" : "No") << "      " << (((castling[1] & two_to_the[1]) != 0)? "Yes" : "No");
                break;
            case 6:
                cout << "|     Black:          " << (((castling[0] & two_to_the[57]) != 0)? "Yes" : "No") << "      " << (((castling[0] & two_to_the[61]) != 0)? "Yes" : "No");
                break;
        }
    }
    cout << "|     En-passant square: ";
    if(en_passant) { print_algebra(bitscan(en_passant)); }
    else { cout << "None"; }

    cout << endl << "     +---+---+---+---+---+---+---+---+\n       A   B   C   D   E   F   G   H" << endl;
}

/* ======================================================================
    Takes a square number [0, .., 63] and prints a square [h1, ..., a8].
   ====================================================================== */

void print_algebra(uint16_t square){
    cout << get_algebra(square);
}

string get_algebra(uint16_t square) {
    string algebra_out = "";
    if((square % 8) == 0)           algebra_out.append("h");
    else if((square % 8) == 1)      algebra_out.append("g");
    else if((square % 8) == 2)      algebra_out.append("f");
    else if((square % 8) == 3)      algebra_out.append("e");
    else if((square % 8) == 4)      algebra_out.append("d");
    else if((square % 8) == 5)      algebra_out.append("c");
    else if((square % 8) == 6)      algebra_out.append("b");
    else if((square % 8) == 7)      algebra_out.append("a");

    algebra_out.append(int_to_string((int)(((square - (square % 8)) / 8) + 1)));
    return algebra_out;
}

/* ==========================================================================================
    Takes a uint16_t move, extracts the source/dest and prints their squares as, e.g., e2e4.
   ========================================================================================== */

void print_move(uint16_t move) {
    if(move == NULL_MOVE) {
        cout << "[null]";
    } else {
        print_algebra(move & MASK_FROM);
        print_algebra((move & MASK_TO) >> 6);
        switch((move & MASK_SPECIAL) >> 12) {
            case 1: cout << "Q"; break;
            case 2: cout << "R"; break;
            case 3: cout << "B"; break;
            case 4: cout << "N"; break;
        }
    }
}

/* ==========================================================================================================
    This will print a move like O-O-O or Nf3, but only if the move is being played on the current gamestate.
   ========================================================================================================== */

string CGame::get_fancy_move(uint16_t move) {
    string      move_out;
    uint16_t    i,
                source          = extract_source(move),
                dest            = extract_dest(move),
                special         = extract_special(move);
    uint8_t     attacker        = board[source],
                defender        = board[dest];
    uint16_t    movelist[256];
    bool        multiples       = false;

    switch(special) {
        case 7: case 8: defender = 1; break;
        case 9: case 11: return "O-O";
        case 10: case 12: return "O-O-O";
    }

    generate_moves(movelist);
    for(i = 0; movelist[i]; i++)
        if((extract_dest(movelist[i]) == dest) && (board[extract_source(movelist[i])] == attacker) && (extract_source(movelist[i]) != source))
            multiples = true;

    switch(attacker) {
    case 2: move_out = "N"; break;  case 3: move_out = "B"; break;  case 4: move_out = "R"; break;  case 5: move_out = "Q"; break;  case 6: move_out = "K"; break;
    }

    if(attacker == 1) {
        if(defender != 0) {
            switch(source % 8) {
                case 0: move_out.append("h"); break;    case 1: move_out.append("g"); break;    case 2: move_out.append("f"); break;    case 3: move_out.append("e"); break;
                case 4: move_out.append("d"); break;    case 5: move_out.append("c"); break;    case 6: move_out.append("b"); break;    case 7: move_out.append("a"); break;
            }
        }
    } else {
        if(multiples) move_out.append(get_algebra(source));
    }

    if(defender) move_out.append("x");

    move_out.append(get_algebra(dest));

    switch(special) {
        case 1: move_out.append("Q"); break;    case 2: move_out.append("R"); break;    case 3: move_out.append("B"); break;    case 4: move_out.append("N"); break;
    }
    return move_out;

}

/* ====================================================================
    Print the move history for the game to the console/command prompt.
   ==================================================================== */

void CGame::print_move_history(){
    for(int i = 1; i <= move_num; i++){
        if(i % 2) { cout << "  " << ((i+1)/2) << ". "; } else { cout << ", "; }
            print_move(hist_moves[i]);
    }
    cout << endl;
}

/* ===================================================================
    Print the FEN of current gamestate to the console/command prompt.
   =================================================================== */

void CGame::print_FEN() {
    int k;
    for(int i = 7; i >= 0; i--) {
        for(int j = 7; j >= 0; j--) {
            switch(board[8*i + j]) {
                case 0: for(k = 1; (board[8*i + (j-1)] == 0) && (j >= 1); j--) k++; cout << k;  break;
                case 1: cout << (((pieces[BLACK][ALL] & two_to_the[8*i + j]) == 0)? "P" : "p"); break;
                case 2: cout << (((pieces[BLACK][ALL] & two_to_the[8*i + j]) == 0)? "N" : "n"); break;
                case 3: cout << (((pieces[BLACK][ALL] & two_to_the[8*i + j]) == 0)? "B" : "b"); break;
                case 4: cout << (((pieces[BLACK][ALL] & two_to_the[8*i + j]) == 0)? "R" : "r"); break;
                case 5: cout << (((pieces[BLACK][ALL] & two_to_the[8*i + j]) == 0)? "Q" : "q"); break;
                case 6: cout << (((pieces[BLACK][ALL] & two_to_the[8*i + j]) == 0)? "K" : "k"); break;
            }
        }
        if(i != 0) cout << "/";
    }
    cout << " " << (wtm? "w" : "b") << " ";
    if(castling[WHITE] | castling[BLACK]) {
        cout << (((castling[WHITE] & two_to_the[1]) != 0)? "K" : "") << (((castling[WHITE] & two_to_the[5]) != 0)? "Q" : "");
        cout << (((castling[BLACK] & two_to_the[57]) != 0)? "k" : "") << (((castling[BLACK] & two_to_the[61]) != 0)? "q " : " ");
    } else
        cout << "- ";
    if(en_passant) print_algebra(bitscan(en_passant));
    else cout << "-";


    cout << " 0 0" << endl;
}

/* ==============================================================================================
    If post is enabled, this is where the engine posts its progress after each search iteration.
   ============================================================================================== */

void CEngine::post_out(CGame * game, uint8_t depth, int score, char high_low_symb) {

    cout << ((int)(depth)) << high_low_symb << " ";                                 /* Depth (4 chars) */
    if(depth < 10) cout << " ";

    if(score < (100 - MATESCORE)) {                                                 /* Score (6 chars) */
        cout << "-M" << (score + MATESCORE) << "  ";
        if(score + MATESCORE < 10) cout << " ";
    } else if(score > (MATESCORE - 100)) {
        cout << " M" << (MATESCORE - score) << "  ";
        if(MATESCORE - score < 10) cout << " ";
    } else {
        if(score >= 0) cout << " ";
        cout << score << " ";
        for(int j = 10; j <= 1000; j*= 10) if(abs(score) < j) cout << " ";
    }

    uint64_t i = (clock() - time_begin_search) / (CLOCKS_PER_SEC / 100);
    cout << i << " ";                                                               /* Time in centiseconds (6 chars) */
    for(uint64_t j = 10; j <= 10000; j *= 10) if(i < j) cout << " ";

    cout << nodes << " ";
    for(uint64_t j = 10; j <= 1000000000; j*= 10) if(nodes < j) cout << " ";        /* Nodes (11 chars) */

    print_pv(game, depth, 0, game->move_num, 0);                                    /* Principal variation */
    post_pretty? cout << endl << endl : cout << endl;

}

/* =================================================================
    Prints various engine statistics to the console/command prompt.
   ================================================================= */

void CEngine::print_engine_stats(){
    cout << "Engine stats:\n\nLevel - MPS: " << time_levels[0] << ", Base: " << time_levels[1] << " seconds, Increment: " << time_levels[2] << " seconds." << endl;
    cout << "Time left: " << time_cs_left << " centiseconds, Time surplus: " << time_surplus << " centiseconds" << endl;
    cout << "Hash table size: " << ((hash.size * 14) / (1024*1024)) << endl;
    cout << "Total nodes searched: " << total_nodes << ", total time searched: " << total_cs_used << ", total NPS: ";
    if(total_cs_used) cout << floor((double)total_nodes / (double)(total_cs_used * 10)) << "K\n";
    else cout << "N/A\n";
    cout << "Estimated branching factor: " << (branching_factor / (double)1) << endl;
}

/* ========================================================================================================================
Print PV is a UI function, which prints the best guess at the principal variation that can be obtained from the table.
======================================================================================================================== */

void CEngine::print_pv(CGame * game, uint8_t depth, uint8_t ply, uint16_t current_move, uint8_t cursor_pos) {

    if((depth == 255) || ((hash.PV_lock[game->zobrist_key % PV_size] != game->zobrist_key)
                      && ((hash.lock[game->zobrist_key % hash.size] != game->zobrist_key)
                      || ((hash.old_depth_flag[game->zobrist_key % hash.size] & HASH_FLAG) == 0))))
        return;

    uint16_t movelist[256], move;
    bool hit = false;

    game->generate_moves(movelist);
    string move_out = (ply != 0)? (post_pretty? "  " : " ") : "";

    if(hash.PV_lock[game->zobrist_key % PV_size] == game->zobrist_key)      move = hash.PV_move[game->zobrist_key % PV_size];
    else if(hash.lock[game->zobrist_key % hash.size] == game->zobrist_key)  move = hash.sugg[game->zobrist_key % hash.size];
    else return;

    for(uint8_t i = 0; movelist[i]; i++) if(move == movelist[i]) hit = true;
    if(!hit) return;

    if((ply == 0) && game->btm) {
        move_out.append(int_to_string((int)((current_move + 1) / 2)));
        move_out.append("... ");
    } else if(game->wtm) {
        move_out.append(int_to_string((int)((current_move + ply + 2) / 2)));
        move_out.append(". ");
    } else {
        cout << ",";
        cursor_pos += 1;
    }

    move_out.append(game->get_fancy_move(move));
    game->make_move(move);

    if(game->is_drawn())
        depth = 0;
    else if(movelist[0] == 0) {
        depth = 0;
        if(game->in_check()) move_out.append("++");
    } else if(game->in_check())
        move_out.append("+");

    if(post_pretty) {
        if(cursor_pos + (uint8_t)move_out.length() >= 80 - (27 + 2 * ((uint8_t)game->btm))) {
            cursor_pos = 0;
            cout << endl << endl << "                         ";
        }
        cursor_pos += (uint8_t)move_out.length();
    }

    cout << move_out;

    print_pv(game, depth - 1, ply + 1, current_move, cursor_pos);
    game->unmake_move();

}

// ================================================================================
//  cin_to_move cin's the users input and converts it to the internal move format.
// ================================================================================

uint16_t cin_to_move(CGame * game) {
    int x;
    unsigned int m[3] = { 0, 0, 5 }, i = 0;
    char m_char[5] = { 0 };

    uint16_t movelist[256];

    for(x = 0; x <= 3; x++) {
        cin >> m_char[x];
        switch(m_char[x]) {
            case 'a': m[x / 2] += 7; break;     case 'b': m[x / 2] += 6; break;     case 'c': m[x / 2] += 5; break;     case 'd': m[x / 2] += 4; break;
            case 'e': m[x / 2] += 3; break;     case 'f': m[x / 2] += 2; break;     case 'g': m[x / 2] += 1; break;     case 'h': m[x / 2] += 0; break;
            case '1': m[x / 2] += 0; break;     case '2': m[x / 2] += 8; break;     case '3': m[x / 2] += 16; break;    case '4': m[x / 2] += 24; break;
            case '5': m[x / 2] += 32; break;    case '6': m[x / 2] += 40; break;    case '7': m[x / 2] += 48; break;    case '8': m[x / 2] += 56; break;
        }
    }

    game->generate_moves(movelist);

    for(i = 0; movelist[i]; i++) {
        if((extract_source(movelist[i]) == m[0]) && (extract_dest(movelist[i]) == m[1])) {
            if((extract_special(movelist[i]) >= 1) && (extract_special(movelist[i]) <= 4)) {
                if(m[2] == 5) {
                    cin >> m_char[4];
                    switch(m_char[4]) {
                        case 'Q': case 'q': m[2]--; case 'R': case 'r': m[2]--; case 'B': case 'b': m[2]--; case 'N': case 'n': m[2]--; break; default: m[2] = 0;
                    }
                }
                if(m[2] == extract_special(movelist[i])) {
                    return movelist[i];

                }
            } else {
                return movelist[i];
            }
        }
    }

    return BOGUS_MOVE;
}

// ==============================================
//  Basic string to and from integer conversion.
// ==============================================

string int_to_string(int in) {
    string out = "";
    int rev = 0;
    for(; in; in = in / 10) rev = (10 * rev) + (in % 10);
    for(; rev; rev = rev / 10) {
        switch(rev % 10) {
            case 0: out.append("0"); break;     case 5: out.append("5"); break;
            case 1: out.append("1"); break;     case 6: out.append("6"); break;
            case 2: out.append("2"); break;     case 7: out.append("7"); break;
            case 3: out.append("3"); break;     case 8: out.append("8"); break;
            case 4: out.append("4"); break;     case 9: out.append("9"); break;
        }
    }
    return out;
}

int string_to_int(string strin) {
    return atoi(strin.c_str());
}

// =======================================================
//  An absolute value function for double variable types.
// =======================================================

double dabs(double x) {
    return (x > 0)? x : -x;
}