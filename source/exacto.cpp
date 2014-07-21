// exacto.cpp
// created: 7:56 PM 12/9/2013
// author: Daniel Sparks
// license: See text/license.txt
// purpose: The main UI loop.

#include "exacto.h"
#include "inlines.h"

/*
    To do list:
    -----------
    * Consider re-writing search algorithm when implementing SMP
    * Implement SMP

    * Update evaluation terms: pieces blocking pawns, "holes," better recognition of backwards pawns.
    * Mobility
    * Refine king safety

    * Tune:
        - Futility pruning
        - Razoring
        - Null move
        - LMR

    * History heuristics, tie it in to the futility pruning, razoring, etc.

*/

int main() {

    cout << "\n   exacto[0.f]  \n <<------------------------------->>  \n             by Daniel Sparks, USA  \n" << endl << endl;
    cout << "Type 'game' for gameplay commands, or 'help' for all commands." << endl << endl;

    int i, j;
    string user_input;

    /* These are the coefficients for the piece square tables (see fill_PST), which can later be optimized 3 at a time. */

    // unoptimized: int16_t C[4][3] = { { 0, 0, 0 }, { 1, 0, 1 }, { 0, 0, 1 }, { 0, 0, 1 } };

    int16_t C[7][3] = { { 0, 0, 0 }, { 1, 0, 1 }, { 0, 0, 1 }, { 0, 0, 1 }, { 0, 0, 0 }, { 0, 0, 0 }, { 1, 0, 0 } };

    CEngine exacto;
    CEngine exacto2;
    CGame game;
    initialize();
    fill_PST(C);
    parse_book("book.bin");
    exacto.book_mode = is_book(true) ? _BOOK_MODE : BOOK_DISABLED;
    game.set_board("Default", "", "", "", "", "");


/*// Consolidate:

    string file[5] = { "tuneraz_11.txt", "tuneraz_12.txt", "tuneraz_13.txt", "tuneraz_14.txt", "tuneraz_15.txt" };
    int score[30] = { 0 };
    uint32_t m = 0, n = 0, r = 0;

    for(int p = 0; p < 5; p++)
        parse_tuning_file(&m, &n, &r, score, file[p]);
    int mu = 0;
    for(int p = 1; p < 27; p++) { cout << p << " : " << (((double)score[p])/2) << endl; mu += score[p]; }

    cout << "Total games: " << mu << endl << "Mean: " << mu / 26 << endl;*/


    for(cin >> user_input; true; cin >> user_input) {
        next_command:

        if(user_input == "quit")
            break;


        else if((user_input == "xboard") || (user_input == "winboard"))
            exacto.post_pretty = false;


        else if(user_input == "protover") {
            cin >> user_input;
            cout << "feature ping=1 setboard=1 playother=1 san=0 usermove=1 time=1 draw=0 sigint=0 sigterm=0 reuse=1 analyze=0 colors=0 ics=0 name=0 pause=0 done=1" << endl;


        } else if(user_input == "accepted") {
            cin >> user_input;


        } else if(user_input == "ping") {
            cin >> user_input;
            cout << "pong " << user_input << endl;


        } else if(user_input == "force") {
            exacto.force = true;
            cout << "Force ON" << endl;


        } else if(user_input == "draw") {
            exacto.draw_offered = true;


        } else if(user_input == "usermove"){
            uint16_t move = cin_to_move(&game);
            if(move != BOGUS_MOVE) {
                game.make_move(move);
                if(!exacto.force) exacto.go(&game);
            }


        } else if(user_input == "go") {
            exacto.go(&game);


        } else if(user_input == "undo") {
            game.unmake_move();


        } else if(user_input == "new") {
            exacto.reset_engine();
            game.set_board("Default", "", "", "", "", "");


        } else if(user_input == "setboard") {
            string POS, WTM, CAS, EPT, HMC, FMC;
            cin >> POS; cin >> WTM; cin >> CAS; cin >> EPT; cin >> HMC; cin >> FMC;
            game.set_board(POS, WTM, CAS, EPT, HMC, FMC);


        } else if(user_input == "post") {
            exacto.post = true;
            cout << "Post ON" << endl;


        } else if(user_input == "nopost") {
            exacto.post = false;
            cout << "Post OFF" << endl;


        } else if(user_input == "version") {
            cout << "0.f.a" << endl;


        } else if(user_input == "bookmode") {
            exacto.book_mode = (exacto.book_mode + 1) % 4;
            if(!is_book(!is_book(false))) continue;
            switch(exacto.book_mode) {
                case 0: cout << "Book mode: BEST_MOVE_ONLY" << endl; break;         case 1: cout << "Book mode: GOOD_MOVES_ONLY" << endl; break;
                case 2: cout << "Book mode: BOOK_LEARNING_MODE" << endl; break;     case 3: cout << "Book mode: DISABLED" << endl; break;
            }


    /*  Winboard/XBoard time input protocol.    */

        } else if(user_input == "level") {
            string temp;
            int MPS, increment, time_in[2] = {0}; cin >> MPS; cin >> temp; cin >> increment;
            bool found_seconds = false;

            for(j = 0; (unsigned int)j < temp.length(); j++) {
                if((temp.substr(j, 1)) == ":") { found_seconds = true; continue; }
                time_in[found_seconds] = (time_in[found_seconds] * 10) + atoi((temp.substr(j,1)).c_str());
            }

            exacto.set_levels(MPS, 60 * time_in[0] + time_in[1], increment);

        } else if(user_input == "st") {
            cin >> i;
            exacto.st(i);

        } else if(user_input == "time") {
            cin >> j;
            exacto.time(j);


        } else if(user_input == "otim") {
            cin >> j;
            exacto.otim(j);


        } else if(user_input == "hash") {
            int n;
            cin >> n;
            exacto.hash.set_dimension(n);


        } else if(user_input == "mt") {
            cin >> i;
            if((1 <= i) && (i <= CPUS)) { exacto.mt = i; cout << "Using " << (int)i << " threads out of " << CPUS << " possible." << endl;}
            else { cout << "Invalid specification, using " << (int)exacto.mt << " threads out of " << CPUS << " possible." << endl; }


        } else if(user_input == "name") {
            cin >> exacto.opponent_name;


    /*  Book interaction.   */

        } else if(user_input == "book_save") {
            cin >> user_input;
            write_book(user_input);


        } else if(user_input == "book+") {
            new_book_entry(&game, cin_to_move(&game));


        } else if(user_input == "book_load") {
            cin >> user_input;
            parse_book(user_input);


        } else if(user_input == "book--") {
            delete_book_entries(game.zobrist_key);


        } else if(user_input == "book_probe") {
            print_book_moves(&game);


        } else if(user_input == "book_addline") {
            for(uint16_t move = cin_to_move(&game); move != BOGUS_MOVE; move = cin_to_move(&game)) {
                new_book_entry(&game, move);
                game.make_move(move);
            }


        } else if(user_input == "book_unlearn") {
            delete_book_learning_data();


        } else if(user_input == "result") {
            cin >> user_input;
            if((user_input.length() != 1) && (exacto.book_mode != BOOK_DISABLED))
                update_book_learning(&game, &exacto, ((uint8_t)(user_input.substr(0, 1) == "1") + 2 * (uint8_t)(user_input.length() != 3)));

            char x; cin >> x;
            if(x == '{')    for(cin >> x; x != '}'; cin >> x) {}
            else {
                cin >> user_input;
                user_input = string(1, x).append(user_input);
                goto next_command;
            }


        } else if(user_input == "book_set_stats") {
            uint16_t x[4];
            x[0] = cin_to_move(&game);
            for(i = 1; i < 4; i++) cin >> x[i];
            set_stats(game.zobrist_key, x[0], x[1], x[2], x[3]);


    /*  Perft and divide are used to test move generation.  */

        } else if(user_input == "perft") {
            cin >> i;
            cout << "perft(" << i << ") = " << (exacto.perft(&game, i, true)-exacto.perft(&game, i-1, true)) << endl;


        } else if(user_input == "divide") {
            cin >> i;
            cout << exacto.divide(&game, i, true) << " total" << endl;
            uint16_t movelist[256];
            game.generate_moves(movelist);
            for(i = 1; movelist[i]; i++) {}
            cout << (int)i << " moves" << endl;


        } else if(user_input == "divide_caps") {
            cin >> i;
            cout << exacto.divide(&game, i, false) << " total" << endl;
            uint16_t movelist[256];
            game.generate_caps(movelist);
            for(i = 1; movelist[i]; i++) {}
            cout << (int)i << " moves" << endl;


    /*  Exacto-specific commands.    */

        } else if(user_input == "makebbfile") {
            make_bitboard_file();
            cout << "Done." << endl;


        } else if(user_input == "print") {
            game.print_board();


        } else if((user_input == "FEN") || (user_input == "print_FEN")) {
            game.print_FEN();


        } else if(user_input == "stats") {
            exacto.print_engine_stats();


        } else if(user_input == "history") {
            game.print_move_history();


        } else if(user_input == "help") {
            cout << endl << "quit, force, usermove, go, undo, new, setboard, post, protover, version, level," << endl;
            cout << "st, time, otim, hash, perft, divide, makebbfile, print, stats, history, help," << endl;
            cout << "game, xboard, book_probe, book+, book--, book_addline, book_unlearn, book_save," << endl;
            cout << "book_load, book_set_stats, result, bookmode, draw" << endl << endl;


        } else if(user_input == "game") {
            cout << endl << "Commands for gameplay:" << endl << "----------------------" << endl;
            cout << "new \t\tStart a new game." << endl;
            cout << "setboard FEN \tSets the position to FEN." << endl;
            cout << "post \t\tToggles whether the engine displays what it is thinking." << endl;
            cout << "print \t\tPrints an ASCII chessboard for your viewing pleasure." << endl;
            cout << "level x y z \tSets time controls: y minutes + z seconds per move." << endl;
            cout << "st x \t\tIgnore levels, use exactly x seconds per move." << endl;
            cout << "go \t\tTells the engine to begin thinking and then play a move." << endl;
            cout << "usermove #### \tUser plays the move ####, and in response the engine" << endl;
            cout <<             "\t\twill begin thinking and make a move of it's own.  ####" << endl;
            cout <<             "\t\tshould be in plain algebra (e.g. e2e4), except for" << endl;
            cout <<             "\t\tpromotions (e.g. e7e8Q)." << endl;
            cout << "quit \t\tClose Exacto." << endl << endl;


    /*  Debugging commands. */

        } else if(user_input == "history_in") {
            game.print_move_history_i();
        } else if(user_input == "print_bbs") {
            game.print_all_bitboards();
        } else if(user_input == "null") {
            game.make_null();
        } else if(user_input == "unnull") {
            game.unmake_null();
        } else if(user_input == "eval_debug") {
            game.print_evaluate();
        } else if(user_input == "suite") {
            uint16_t x;
            cin >> x;

            cout << "Reticulating splines... ";
            run_suite(&exacto, x);
            cout << "Done." << endl;

        } else if(user_input == "playself") {
            int16_t x[3];
            for(i = 0; i < 3; i++) cin >> x[i];
            cin >> user_input;
            crude_tune(&exacto, &exacto2, x[0], x[1], x[2], user_input);

        } else if(user_input == "move_order") {
            uint16_t movelist[256];
            int16_t sortlist[256];
            game.generate_moves(movelist);
            exacto.order_moves(&game, movelist, sortlist, 0);
            for(i = 0; movelist[i]; i++) {
                cout << game.get_fancy_move(movelist[i]) << "  "; if(i % 4 == 3) cout << endl;
            }
        } else if(user_input == "SEE") {
            uint16_t movelist[256];
            game.generate_caps(movelist);
            for(i = 0; movelist[i]; i++) cout << game.get_fancy_move(movelist[i]) << " " << game.SEE((uint8_t)extract_source(movelist[i]), (uint8_t)extract_dest(movelist[i])) << endl;


        } else
            cout << "Error (unknown command): " << user_input << endl;

    }

    return 0;
}
