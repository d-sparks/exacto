// tune.cpp
// created: 10:36PM 3/15/14
// author: Daniel Sparks
// license: See text/license.txt
// purpose: Functions meant to tune search and evaluation parameters.

#include "exacto.h"

/* // Consolidate:

    string file[4] = { "tunefile_1.txt", "tunefile_2.txt", "tunefile_3.txt", "tunefile_4.txt" };
    int score[30] = { 0 };
    uint32_t m = 0, n = 0, r = 0;

    for(int p = 0; p < 4; p++)
        parse_tuning_file(&m, &n, &r, score, file[p]);
    int mu = 0;
    for(int p = 1; p < 27; p++) { cout << p << " : " << (((double)score[p])/2) << endl; mu += score[p]; }

    cout << "Total games: " << mu << endl << "Mean: " << mu / 26 << endl;

*/


// ========================================================================
//  If crude_tune needs to set somethings between games, put them in here.
// ========================================================================

void set_player(unsigned int player) {
    RAZOR_MAX_DEPTH = (player / 3) + 3;
    RAZOR_MARGIN = ((player % 3) + 1) * 50;
}

// ==================================================
//  Plays n * (n-1) * m games, here m = 4 and n = 8.
// ==================================================

void crude_tune(CEngine * engine_1, CEngine * engine_2, int16_t MPS, int16_t base, int16_t increment, string from_file) {

    string filename = "logs/tuneraz.txt";
    ofstream file;

    /*int16_t C_basic[4][3] = { { 0, 0, 0 }, { 2, 1, 2 }, { 0, 0, 1 }, { 0, 0, 1 } };
    int16_t C[2][4][3] = { 0 };

    for(int i = 0; i <= 1; i++) {
        for(int j = PAWN; j <= BISHOP; j++) {
            C[i][j][0] = C_basic[j][0];
            C[i][j][1] = C_basic[j][1];
            C[i][j][2] = C_basic[j][2];
        }
    }*/

    unsigned int C[2];

    int score[30] = { 0 };
    uint32_t i = 1, j = 2, k = 0;
    bool skip_first = false;

    if(from_file == "new") {
        file.open(filename, std::ios_base::app);
        file << "S " << MPS << " " << base << " " << increment << " ";
        file.close();
    } else {
        string buffer;

        parse_tuning_file(&k, &i, &j, score, from_file);
        ifstream get_levels;
        get_levels.open(from_file);

        get_levels >> buffer;
        if(buffer == "S") {
            get_levels >> buffer; MPS = string_to_int(buffer);
            get_levels >> buffer; base = string_to_int(buffer);
            get_levels >> buffer; increment = string_to_int(buffer);
        }
        get_levels.close();

        filename = from_file;
        skip_first = true;
    }

    for(k = (skip_first? k : 0); k < 4; k++) {
        for(i = (skip_first? i : 1); i < 11; i++) {
            for(j = (skip_first? j : 1); j < 11; j++) {

                if(skip_first) { skip_first = false; continue; }

                if((i == j) || ((i != 2) && (i != 3) && (i != 7) && (i != 8) && (i != 9))
                            || ((j != 2) && (j != 3) && (j != 7) && (j != 8) && (j != 9)))
                    continue;

                engine_1->reset_engine();
                engine_2->reset_engine();

                /*C[k % 2][KNIGHT][0] = i % 3;
                C[k % 2][KNIGHT][1] = (i / 3) % 3;
                C[k % 2][KNIGHT][2] = (i / 9) + 1;
                C[(k + 1) % 2][KNIGHT][0] = j % 3;
                C[(k + 1) % 2][KNIGHT][1] = (j / 3) % 3;
                C[(k + 1) % 2][KNIGHT][2] = (j / 9) + 1;*/

                C[k % 2] = i;
                C[(k + 1) % 2] = j;

                // If k is odd, then i corresponds to white and j to black
                // If k is even, then i corresponds to black and j to white

                int result = play_self(engine_1, engine_2, MPS, base, increment, C);

                if(k % 2) cout << (int)i << " vs " << (int)j << " - ";
                else cout << (int)j << " vs " << (int)i << " - ";
                if(result == 0) cout << "1/2-1/2" << endl;
                else cout << ((result > 0) ? "1-0" : "0-1") << endl;

                file.open(filename, std::ios_base::app);
                    file << (int)i << " " << (int)j << " ";
                    if(result == 0) file << "= ";
                    else if((k % 2) == ((result + 1) / 2)) file << "+ ";
                    else if((k % 2) != ((result + 1) / 2)) file << "- ";
                file.close();

                if(result) {
                    if((k % 2) == ((result + 1) / 2)) score[i]++;
                    else score[j]++;
                }

            }
        }
    }

    unsigned int best = 0;
    for(unsigned int i = 1; i < 26; i++) if(score[i] > score[best]) best = i;

    cout << "Best index: " << best << endl;

}


int play_self(CEngine * engine_1, CEngine * engine_2, int16_t MPS, int16_t base, int16_t increment, /*int16_t C[2][4][3]*/ unsigned int C[2]) {
    uint16_t movelist[256];

    CEngine * players[2] = { engine_1, engine_2 };

    int result;

    CGame game;

    game.set_board("Default", "", "", "", "", "");
    players[0]->reset_engine();
    players[1]->reset_engine();
    players[0]->set_levels(MPS, base, increment);
    players[1]->set_levels(MPS, base, increment);

    while(true) {
        //fill_PST(C[game.wtm]); /* Or call some function, for instance, or set something to something, etc. */
        set_player(C[game.wtm]);
        players[game.wtm]->go(&game);
        game.generate_moves(movelist);
        if((movelist[0] == 0) && game.in_check()) {
            result = 1 - 2 * (int)game.wtm;
            break;
        } else if((movelist[0] == 0) || (game.is_drawn()))  {
            result = 0;
            break;
        } else if(players[!game.wtm]->time_cs_left <= 0) {
            result = 2 * (int)game.wtm - 1;
            break;
        }
    }

    update_book_learning(&game, engine_1, (result == 0) ? UNASSIGNED : (result + 1) / 2);
    update_book_learning(&game, engine_2, (result == 0) ? UNASSIGNED : (result + 1) / 2);

    return result;

}

void parse_tuning_file(uint32_t * k, uint32_t * i, uint32_t * j, int score[30], string filename) {
    ifstream file;
    file.open(filename);
    string buffer;

    cout << "Opened " << filename;

    uint8_t player[2], first[2] = { 200, 201 };
    int score_buffer[30] = { 0 };
    int game_count = 0;
    bool detect_first = true;

    (*k) = 4294967295;

    for(file >> buffer; !file.eof(); file >> buffer) {
        if(buffer == "S") { file >> buffer; file >> buffer; file >> buffer; continue; }
        player[0] = string_to_int(buffer); file >> buffer;
        player[1] = string_to_int(buffer); file >> buffer;

        if(detect_first) {
            first[0] = player[0];
            first[1] = player[1];
        }

        if((player[0] == first[0]) && (player[1] == first[1])) {
            (*k)++;
            for(int j = 0; j < 30; j++) score[j] += score_buffer[j];
            for(int j = 0; j < 30; j++) score_buffer[j] = 0;
        }

        (*i) = player[0];
        (*j) = player[1];
        if(buffer == "+") { score_buffer[player[0]] += 2; game_count++; }
        if(buffer == "-") { score_buffer[player[1]] += 2; game_count++; }
        if(buffer == "=") {
            score_buffer[player[0]]++;
            score_buffer[player[1]]++;
            game_count++;
        }

        detect_first = false;
    }

    cout << ", " << game_count << " games parsed." << endl;
}

uint16_t algebra_to_move(string move) {
    uint16_t move_out[2] = { 0 };
    if(move.length() != 4) return 0;

    for(uint8_t i = 0; i <= 2; i += 2) {
        if(move.substr(i, 1) == "a") move_out[i / 2] += 7;
        if(move.substr(i, 1) == "b") move_out[i / 2] += 6;
        if(move.substr(i, 1) == "c") move_out[i / 2] += 5;
        if(move.substr(i, 1) == "d") move_out[i / 2] += 4;
        if(move.substr(i, 1) == "e") move_out[i / 2] += 3;
        if(move.substr(i, 1) == "f") move_out[i / 2] += 2;
        if(move.substr(i, 1) == "g") move_out[i / 2] += 1;
        move_out[i / 2] += 8 * (uint16_t)string_to_int(move.substr(i + 1, 1)) - 8;
    }

    return move_out[0] | (move_out[1] << 6);
}

uint16_t set_suite_position(CGame * game, uint16_t position_number) {
    switch(position_number) {
        case 1: game->set_board("r3r1k1/pp3pbp/1qp3p1/2B5/2BP2b1/Q1n2N2/P4PPP/3R1K1R", "b", "-", "-", "0", "0");        /* Fischer Be6*/
            return algebra_to_move("g4e6");
        case 2: game->set_board("r1b2rk1/p3nppp/1pn1p3/2b3B1/2B1N1q1/3Q1N2/PPP2PPP/2KR3R", "w", "-", "-", "0", "0");    /* Tal Bf6, Crafty h3 */
            return algebra_to_move("h2h3");
        case 3: game->set_board("4r1k1/1bqnrpbp/pp1pp1p1/6P1/Pn2PP2/1NN1B2R/1PP2QBP/5RK1", "b", "-", "-", "0", "0");    /* Kasparov f5 */
            return algebra_to_move("f7f5");
        case 4: game->set_board("1rb3k1/p1p3pp/2pb4/3p4/3Pp3/7q/PPPQBB1P/2KR3R", "b", "-", "-", "0", "0");              /* Morphy/Crafty: Qa3 */
            return algebra_to_move("h3a3");
        case 5: game->set_board("1r3rk1/pbpn2qp/1p1p1np1/3P1p2/1P5N/5PPB/PB5P/2QR1RK1", "w", "-", "-", "0", "0");       /* Alekhin/Crafty Bxf5 */
            return algebra_to_move("h3f5");
        case 6: game->set_board("2kr3r/pppq2b1/3p2pp/1N3b2/2P4P/4B3/P1nQBPP1/R4RK1", "w", "-", "-", "0", "0");          /* Kasparov/Crafty Bf3 */
            return algebra_to_move("e2f3");
        case 7: game->set_board("Q7/4q3/2pq4/4p3/1kPpP1P1/2nP4/7Q/5BK1", "b", "-", "-", "0", "0");                      /* Tal/Crafty Kb3 */
            return algebra_to_move("b4b3");
        case 8: game->set_board("1k1r4/pp1b1R2/3q2pp/4p3/2B5/4Q3/PPP2B2/2K5", "b", "-", "-", "0", "0");                 /* BK.01 : Qd1+ */
            return algebra_to_move("d6d1");
        case 9: game->set_board("3r1k2/4npp1/1ppr3p/p6P/P2PPPP1/1NR5/5K2/2R5", "w", "-", "-", "0", "0");                /* BK.02 : d5 */
            return algebra_to_move("d4d5");
        case 10: game->set_board("2q1rr1k/3bbnnp/p2p1pp1/2pPp3/PpP1P1P1/1P2BNNP/2BQ1PRK/7R", "b", "-", "-", "0", "0");  /* BK.03 : f5 */
            return algebra_to_move("f6f5");
        case 11: game->set_board("rnbqkb1r/p3pppp/1p6/2ppP3/3N4/2P5/PPP1QPPP/R1B1KB1R", "w", "-", "-", "0", "0");       /* BK.04 : e6 */
            return algebra_to_move("e5e6");
        case 12: game->set_board("r1b2rk1/2q1b1pp/p2ppn2/1p6/3QP3/1BN1B3/PPP3PP/R4RK1", "w", "-", "-", "0", "0");       /* BK.05 : a4 */
            return algebra_to_move("a2a4");
        case 13: game->set_board("2r3k1/pppR1pp1/4p3/4P1P1/5P2/1P4K1/P1P5/8", "w", "-", "-", "0", "0");                 /* BK.06 : g6 */
            return algebra_to_move("g5g6");
        case 14: game->set_board("1nk1r1r1/pp2n1pp/4p3/q2pPp1N/b1pP1P2/B1P2R2/2P1B1PP/R2Q2K1", "w", "-", "-", "0", "0");    /* BK.07 : Nf6 */
            return algebra_to_move("h5f6");
        case 15: game->set_board("4b3/p3kp2/6p1/3pP2p/2pP1P2/4K1P1/P3N2P/8", "w", "-", "-", "0", "0");                  /* BK.08 : f5 */
            return algebra_to_move("f4f5");
        case 16: game->set_board("2kr1bnr/pbpq4/2n1pp2/3p3p/3P1P1B/2N2N1Q/PPP3PP/2KR1B1R", "w", "-", "-", "0", "0");        /* BK.09 : f5 */
            return algebra_to_move("f4f5");
        case 17: game->set_board("3rr1k1/pp3pp1/1qn2np1/8/3p4/PP1R1P2/2P1NQPP/R1B3K1", "b", "-", "-", "0", "0");            /* BK.10 : Ne5 */
            return algebra_to_move("c6e5");
        case 18: game->set_board("2r1nrk1/p2q1ppp/bp1p4/n1pPp3/P1P1P3/2PBB1N1/4QPPP/R4RK1", "w", "-", "-", "0", "0");       /* BK.11 : f4 */
            return algebra_to_move("f2f4");
        case 19: game->set_board("r3r1k1/ppqb1ppp/8/4p1NQ/8/2P5/PP3PPP/R3R1K1", "b", "-", "-", "0", "0");                   /* BK.12 : Bf5 */
            return algebra_to_move("d7f5");
        case 20: game->set_board("r2q1rk1/4bppp/p2p4/2pP4/3pP3/3Q4/PP1B1PPP/R3R1K1", "w", "-", "-", "0", "0");              /* BK.13 : b4 */
            return algebra_to_move("b2b4");
        case 21: game->set_board("rnb2r1k/pp2p2p/2pp2p1/q2P1p2/8/1Pb2NP1/PB2PPBP/R2Q1RK1", "w", "-", "-", "0", "0");            /* BK.14 : Qd2 */
            return algebra_to_move("d1d2");
        case 22: game->set_board("2r3k1/1p2q1pp/2b1pr2/p1pp4/6Q1/1P1PP1R1/P1PN2PP/5RK1", "w", "-", "-", "0", "0");          /* BK.15 : Qxg7+ */
            return algebra_to_move("g4g7");
        case 23: game->set_board("r1bqkb1r/4npp1/p1p4p/1p1pP1B1/8/1B6/PPPN1PPP/R2Q1RK1", "w", "-", "-", "0", "0");          /* BK.16 : Ne4 */
            return algebra_to_move("d2e4");
        case 24: game->set_board("r2q1rk1/1ppnbppp/p2p1nb1/3Pp3/2P1P1P1/2N2N1P/PPB1QP2/R1B2RK1", "b", "-", "-", "0", "0");  /* BK.17 : h5 */
            return algebra_to_move("h7h5");
        case 25: game->set_board("r1bq1rk1/pp2ppbp/2np2p1/2n5/P3PP2/N1P2N2/1PB3PP/R1B1QRK1", "b", "-", "-", "0", "0");      /* BK.18 : Nb3 */
            return algebra_to_move("c5d3");
        case 26: game->set_board("3rr3/2pq2pk/p2p1pnp/8/2QBPP2/1P6/P5PP/4RRK1", "b", "-", "-", "0", "0");                   /* BK.19 : Rxe4 */
            return algebra_to_move("e8e4");
        case 27: game->set_board("r4k2/pb2bp1r/1p1qp2p/3pNp2/3P1P2/2N3P1/PPP1Q2P/2KRR3", "w", "-", "-", "0", "0");          /* BK.20 : g4 */
            return algebra_to_move("g3g4");
        case 28: game->set_board("3rn2k/ppb2rpp/2ppqp2/5N2/2P1P3/1P5Q/PB3PPP/3RR1K1", "w", "-", "-", "0", "0");             /* BK.21 : Nh6 */
            return algebra_to_move("f5h6");
        case 29: game->set_board("2r2rk1/1bqnbpp1/1p1ppn1p/pP6/N1P1P3/P2B1N1P/1B2QPP1/R2R2K1", "b", "-", "-", "0", "0");        /* BK.22 : Bxe4 */
            return algebra_to_move("b7e4");
        case 30: game->set_board("r1bqk2r/pp2bppp/2p5/3pP3/P2Q1P2/2N1B3/1PP3PP/R4RK1", "b", "-", "-", "0", "0");                /* BK.23 : f6 */
            return algebra_to_move("f7f6");
        case 31: game->set_board("r2qnrnk/p2b2b1/1p1p2pp/2pPpp2/1PP1P3/PRNBB3/3QNPPP/5RK1", "w", "-", "-", "0", "0");       /* BK.15 : f4 */
            return algebra_to_move("f2f4");

    }

    return 0;


}

void run_suite(CEngine * engine, uint16_t seconds_per_position) {

    ofstream text;
    string filename = "logs/test_results.txt";
    text.open(filename);

    text << "Testing suite at " << seconds_per_position << " seconds per position." << endl << endl;

    double avg_depth = 0, avg_branching_factor = 0, avg_kNPS = 0;

    bool post = engine->post;

    CGame game;
    uint16_t correct_move, solved = 0, i = 1;

    text << endl;

    for(; i <= 31; i++) {
        engine->reset_engine();
        engine->st(seconds_per_position);
        engine->post = post;
        correct_move = set_suite_position(&game, i);
        engine->go(&game);
        if((game.hist_moves[game.move_num] & MASK_SQUARES) == correct_move)
            solved++;

        if(post) {
            cout << endl << endl << "Correct move: "; print_move(correct_move);
            if((game.hist_moves[game.move_num] & MASK_SQUARES) == correct_move)
                cout << " (SOLVED)" << endl;
            else
                cout << " (NOT SOLVED)" << endl;
            engine->print_engine_stats();
            cout << endl;
        }

        text << endl << "Position " << i << endl << endl;
        text << "Correct move: " << get_algebra(extract_source(correct_move)) << get_algebra(extract_dest(correct_move)) << endl;
        text << "Move chosen: " << get_algebra(extract_source(game.hist_moves[game.move_num])) << get_algebra(extract_dest(game.hist_moves[game.move_num])) << endl;

        game.unmake_move();

        text << "Depth: " << ((engine->hash.old_depth_flag[game.zobrist_key % engine->hash.size] & HASH_DEPTH) >> 2) << endl;
        text << "Branching factor: " << engine->branching_factor << endl;

        if(engine->total_cs_used)
            text << "NPS: " << (engine->total_nodes / (10 * engine->total_cs_used)) << endl;

        avg_depth += ((engine->hash.old_depth_flag[game.zobrist_key % engine->hash.size] & HASH_DEPTH) >> 2);
        avg_branching_factor += engine->branching_factor;
        if(engine->total_cs_used) avg_kNPS += ((double)engine->total_nodes / (10 * engine->total_cs_used));
        else if(i > 1) avg_kNPS += (avg_kNPS / (i - 1));

        text << endl << "-------------------------------------------------------------------------------" << endl;

    }


    text << endl << endl << "RESULTS:" << endl << endl;
    text << "Average depth: " << (avg_depth / (i - 1)) << endl;
    text << "Average branching factor: " << (avg_branching_factor / (i - 1)) << endl;
    text << "Average kNPS: " << (avg_kNPS / (i - 1)) << endl;
    text << "Solved percentage: " << ((double)solved / (i - 1)) << " (" << solved << "/" << (i - 1) << ")" << endl;


    if(post) {
        cout << endl << endl << "RESULTS:" << endl << endl;
        cout << "Average depth: " << (avg_depth / (i - 1)) << endl;
        cout << "Average branching factor: " << (avg_branching_factor / (i - 1)) << endl;
        cout << "Average kNPS: " << (avg_kNPS / (i - 1)) << endl;
        cout << "Solved percentage: " << ((double)solved / (i - 1)) << " (" << solved << "/" << (i - 1) << ")" << endl;
    }

    text.close();

}