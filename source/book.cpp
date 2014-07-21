// book.cpp
// created: 10:42 PM 12/22/13
// author: Daniel Sparks
// license: See text/license.txt
// purpose: Interact with the opening book.

#include "exacto.h"
#include "inlines.h"

uint32_t book_entries;
uint64_t * book_position;
uint16_t * book_move;
uint16_t * book_quality;
uint32_t * book_learn;

// ============================================================
//  Using <random> slightly better distributions are possible.
// ============================================================

uint16_t select_random_move(uint16_t * movelist) {
    uint8_t i = 0;
    while(movelist[i]) i++;

    if(i == 1) return movelist[0];

    double random_select = (i - 1) * ((double)rand() / (double)RAND_MAX);

    return movelist[(uint16_t)round(random_select - 0.5)];
}


// ====================================================================================
//  Prase book reads book.book from binary, and places it in a hash table on the heap.
// ====================================================================================

void parse_book(string filename) {

    char buffer[16];

    uint64_t position;
    uint16_t move;
    uint16_t quality;
    uint32_t learn;
    uint64_t x = 0, y;

    /*  First, run through the book and see how many entrines it has.   */

    book_entries = 100;
    ifstream book(filename, ios::in | ios::binary);
    for(book.read(buffer, 16); book; book.read(buffer, 16)) book_entries += 2;

    book.clear();
    book.seekg(0);

    /*  Redimension the book table accordingly. */

    delete book_position;
    delete book_move;
    delete book_quality;
    delete book_learn;

    book_position = new uint64_t[book_entries];
    book_move = new uint16_t[book_entries];
    book_quality = new uint16_t[book_entries];
    book_learn = new uint32_t[book_entries];

    /*  Write 0's to the lock entries in the book table.    */

    for(x = 0; x < book_entries; x++)
        book_position[x] = 0;

    /*  If there are no entries, break. */

    if(book_entries == 100) return;

    /*  Now parse the book.  If there is a type 2 collision, the index will advance forward to
        the first open spot in the book and place the position there.  The table has twice as many
        slots as entries, so space will not be a problem.  When probing the book, must look at
        all entries until a zero entry is read. */

    for(book.read(buffer, 16), x = 0; book; book.read(buffer, 16), x += 2) {
        position = *(uint64_t *)&buffer[0];
        move = *(uint16_t *)&buffer[8];
        quality = *(uint16_t *)&buffer[10];
        learn = *(uint32_t *)&buffer[12];

        for(y = 0; book_position[(position + y) % book_entries] != 0;) y++;

        book_position[(position + y) % book_entries] = position;
        book_move[(position + y) % book_entries] = move;
        book_quality[(position + y) % book_entries] = quality;
        book_learn[(position + y) % book_entries] = learn;
    }

    /*  For choosing random book moves. */

    srand((int)time(NULL));
    for(int x = 0; x < 100; x++) rand();

}


// =======================================================================================
//  This function simply returns true if a book has been loaded into the book hash table.
// =======================================================================================

bool is_book(bool post) {
    uint16_t y = 0;
    for(uint16_t x = 0; x < book_entries; x++) {
        if(book_position[x] != 0) y++;
    }
    if(post && (y != 0)) cout << "Opening book loaded successfully: " << y << " moves ready." << endl << endl;
    else if(post) cout << "No opening book loaded." << endl << endl;
    if(y) return true;
    else return false;
}


// =========================================================================
//  Add a new entry into the opening book (make sure there is space first).
// =========================================================================

void new_book_entry(CGame * game, uint16_t move) {
    uint64_t x = 0;

    while(book_position[(game->zobrist_key + x) % book_entries]) x++;

    book_position[(game->zobrist_key + x) % book_entries] = game->zobrist_key;
    book_move[(game->zobrist_key + x) % book_entries] = move;
    book_quality[(game->zobrist_key + x) % book_entries] = 0;
    book_learn[(game->zobrist_key + x) % book_entries] = 0;
}


// ============================================================
//  Write the current book to the hard drive in binary format.
// ============================================================

void write_book(string filename) {

    char buffer[16];

    ofstream book(filename, ios::out | ios::binary);


    for(uint64_t x = 0; x < book_entries; x++) {
        if(book_position[x] == 0) continue;
        *(uint64_t *)&buffer[0] = book_position[x];
        *(uint16_t *)&buffer[8] = book_move[x];
        *(uint16_t *)&buffer[10] = book_quality[x];
        *(uint16_t *)&buffer[12] = book_learn[x];

        book.write(buffer, 16);
    }

    book.close();
}

// ====================================================
//  Deletes all book entries with a given zobrist key.
// ====================================================

void delete_book_entries(uint64_t key) {
    char x;
    cout << "You are about to delete all book entries associated to this position." << endl << endl << "Are you sure? (y/n)" << endl;
    cin >> x;
    if(x == 'y') for(uint64_t x = 0; x < book_entries; x++) if(book_position[x] == key) book_position[x] = 0;
}

// ==========================================================================
//  Deletes all learning data from the book, and saves a backup under logs/.
// ==========================================================================

void delete_book_learning_data() {
    char x;
    cout << "You are about to delete all book learning data.  The moves and lines will remain the same.  ";
    cout << "If you do this, you'll need to turn exacto onto learning mode with the 'learning' command in order ";
    cout << "to repopulate the learning entries  .A backup of the current book with learning data will be saved ";
    cout << "under logs / book.back, but note that only one backup will be saved at a time." << endl << endl;
    cout << "Are you sure? (y/n)" << endl;
    cin >> x;
    write_book("logs/book.back");
    if(x == 'y') for(uint64_t x = 0; x < book_entries; x++) {
        book_learn[x] = 0;
        book_quality[x] = 0;
    }
    write_book("book.bin");
}

// ============================================================
//  Set the learning statistics for a given move and position.
// ============================================================

void set_stats(uint64_t key, uint16_t move, uint16_t wins, uint16_t losses, uint16_t draws) {
    uint16_t x = 0;
    for(; x < book_entries; x++) if((book_position[(key + x) % book_entries] == key) && (book_move[(key + x) % book_entries] == move)) break;

    if(x == book_entries) return;
    cout << "WINS IN: " << wins << " LOSSES IN:" << losses << " DRAWS IN:" << draws << endl;
    book_quality[(key + x) % book_entries] = wins;
    book_learn[(key + x) % book_entries] = losses + ((uint32_t)draws << 16);
}

// ===========================================================================================
//  Probes the book.  Places moves in quality in the given arrays, and returns the best move.
// ===========================================================================================

uint16_t probe_book(uint64_t key, uint16_t move_array[11], uint16_t quality_array[11], uint32_t learn_array[11]) {
    uint64_t x = 0;
    uint16_t best_move = BOGUS_MOVE, best_quality = 0, i = 0;

    for(; book_position[(key + x) % book_entries] != 0; x++) {
        if(x == book_entries) break;
        if(book_position[(key + x) % book_entries] != key) continue;

        move_array[i++] = book_move[(key + x) % book_entries];
        quality_array[i - 1] = book_quality[(key + x) % book_entries];
        learn_array[i - 1] = book_learn[(key + x) % book_entries];

        if(book_quality[(key + x) % book_entries] >= best_quality) {
            best_quality = book_quality[(key + x) % book_entries];
            best_move = book_move[(key + x) % book_entries];
        }

    }

    move_array[i] = 0;

    return best_move;
}

// ===========================================================================
//  Probes the book, and prints the book suggestions to the console/terminal.
// ===========================================================================

void print_book_moves(CGame * game) {
    uint16_t movelist[256] = { 0 };
    uint16_t qualitylist[256];
    uint32_t learnlist[256];

    if(probe_book(game->zobrist_key, movelist, qualitylist, learnlist) != BOGUS_MOVE)
        cout << endl;

    double quality, win_ratio, out_of_book_ratio;
    uint16_t wins, losses, draws;



    for(uint8_t i = 0; movelist[i]; i++) {
        wins = qualitylist[i]; losses = learnlist[i] & MASK_16BITS; draws = learnlist[i] >> 16;

        if(learnlist[i] != 0) out_of_book_ratio = (double)qualitylist[i] / (qualitylist[i] + (learnlist[i] & MASK_16BITS) + (learnlist[i] >> 16));
        else out_of_book_ratio = (qualitylist[i] ? 1 : 2);

        game->make_move(movelist[i]);
        quality = find_worst_stats(game, &wins, &losses, &draws);
        game->unmake_move();

        win_ratio = wins ? ((double)wins / (wins + losses + draws)) : 0;

        quality = min(quality, out_of_book_ratio);

#ifdef _BOOK_AVERAGING
        quality = max(quality, win_ratio);
#endif

        cout << (int)(i + 1) << ". Book:  " << game->get_fancy_move(movelist[i]) << " (best line: ";
        if((int)dabs(quality) == 2) cout << "?";
        else cout << quality;
        cout << ", overall: " << wins << "-" << losses << "-" << draws << ")" << endl << endl;
    }

}

// ======================================================================================
//  When a game is over, this function is called to update the book-learning statistics.
// ======================================================================================

void update_book_learning(CGame * game, CEngine * engine, uint8_t winner) {

    if(engine->engine_color == SWITCHED || engine->engine_color == UNASSIGNED) return;

    uint64_t position;
    uint16_t move, y;
    uint16_t x = 1 - engine->engine_color, last_book_move = 0;

    for(; x <= game->move_num; x += 2) {
        position = game->hist_zobrist[x];
        move = game->hist_moves[x + 1];
        for(y = 0; book_position[(position + y) % book_entries]; y++) {
            if((book_position[(position + y) % book_entries] == position)
                && (book_move[(position + y) % book_entries] == move))

                last_book_move = x;
        }
        if(x > last_book_move) break;
    }

    if(last_book_move) {
        position = game->hist_zobrist[last_book_move];
        move = game->hist_moves[last_book_move + 1];
        for(y = 0; book_position[(position + y) % book_entries]; y++) {
            if((book_position[(position + y) % book_entries] == position)
                && (book_move[(position + y) % book_entries] == move)) {

                /* Update the statistics */

                if(winner == engine->engine_color) book_quality[(position + y) % book_entries]++;
                else if(winner == UNASSIGNED) book_learn[(position + y) % book_entries] += (MASK_16BITS + 1);
                else if(winner == (1 - engine->engine_color)) book_learn[(position + y) % book_entries]++;

            }
        }
    }

    uint16_t movelist[11], otherlist[11]; //, surprise_move = game->hist_moves[last_book_move + 2];
    uint32_t learnlist[11];
    probe_book(game->hist_zobrist[last_book_move + 1], movelist, otherlist, learnlist);
    bool found_surprise = (winner != engine->engine_color);
    if(movelist[0]) for(x = 0; movelist[x]; x++) if(movelist[x] == game->hist_moves[last_book_move + 2]) found_surprise = false;

    /*  Make a PGN log of the game. */

    ofstream text;
    string temp[8] = { "a", "b", "c", "d", "e", "f", "g", "h" };
    string file_out = (found_surprise? "logs/surprise-" : "logs/game-");
    uint64_t digits = (game->hist_zobrist[game->move_num]);
    for(; digits & 7; digits >>= 8)
        file_out.append(temp[digits & 7]);
    file_out.append(".txt");

    text.open(file_out);

    text << "[White: \"" << (engine->engine_color ? "Exacto" : engine->opponent_name) << "\"]" << endl;
    text << "[Black: \"" << (engine->engine_color ? engine->opponent_name : "Exacto") << "\"]" << endl;
    if(winner == UNASSIGNED) text << "[Result: \"1/2-1/2\"]" << endl;
    else text << (winner ? "[Result: \"1-0\"]" : "[Result: \"0-1\"]") << endl;
    text << "[Level: \"" << (engine->time_levels[1] / 60) << ":" << setfill('0') << setw(2) << (engine->time_levels[1] % 60) << " + " << engine->time_levels[2] << " sec / move\"]" << endl;
    text << endl;

    CGame local_game;
    local_game.set_board("Default", "", "", "", "", "");
    for(x = 1; x <= game->move_num; x++) {
        if((x % 2) == 1) text << (x + 1) / 2 << ". ";
        else text << ", ";
        text << local_game.get_fancy_move(game->hist_moves[x]) << " ";

        if((x == (last_book_move + 2)) && found_surprise) text << "{Surprise!} ";

        local_game.make_move(game->hist_moves[x]);
    }
    text.close();


#ifdef _BOOK_LEARNING
    write_book("book.bin");
#endif

}

// =====================================================================================================================================
//  Find best/worst stats are a type of minimax algorithm, which determines the quality of a move in the book (based on learning data).
// =====================================================================================================================================

double find_best_stats(CGame * game, uint8_t random_mode, uint16_t * best_move, uint16_t * wins, uint16_t * losses, uint16_t * draws) {
    uint16_t movelist[11];
    uint16_t qualitylist[11];
    uint32_t learnlist[11];
    double exact_val[11];

    *best_move = BOGUS_MOVE;

    if(probe_book(game->zobrist_key, movelist, qualitylist, learnlist) == BOGUS_MOVE) {
        return 2;
    }

    double best = -2, win_ratio, out_of_book_ratio;
    uint16_t i, my_wins, my_losses, my_draws;

    for(i = 0; movelist[i]; i++) {
        my_wins = qualitylist[i]; my_losses = learnlist[i] & MASK_16BITS; my_draws = learnlist[i] >> 16;

        if(learnlist[i]) out_of_book_ratio = (double)qualitylist[i] / (qualitylist[i] + (learnlist[i] & MASK_16BITS) + (learnlist[i] >> 16));
        else out_of_book_ratio = (qualitylist[i] ? 1 : 2);

        game->make_move(movelist[i]);
        exact_val[i] = find_worst_stats(game, &my_wins, &my_losses, &my_draws);
        game->unmake_move();

        win_ratio = my_wins ? ((double)my_wins / (my_wins + my_losses + my_draws)) : 0;

        exact_val[i] = min(out_of_book_ratio, exact_val[i]);

#ifdef _BOOK_AVERAGING
        exact_val[i] = max(win_ratio, exact_val[i]);
#endif


        (*wins) += my_wins; (*losses) += my_losses; (*draws) += my_draws;

        exact_val[i] = min(win_ratio, dabs(exact_val[i]));

        if(exact_val[i] > best) {
            best = exact_val[i];
            *best_move = movelist[i];
        }
    }

    best = max(best, (my_wins ? (double)my_wins / (my_wins + my_losses + my_draws) : 0));

    /*  Sometimes you don't want to play the best move.  We handle those cases here.    */

    if(random_mode == BOOK_GOOD_ONLY) {
        uint16_t goodmoves[11];
        uint8_t k = 0;
        for(i = 0; movelist[i]; i++) if(exact_val[i] >= 0.875*best) goodmoves[k++] = movelist[i];
        goodmoves[k] = 0;
        *best_move = select_random_move(goodmoves);
    } else if(random_mode == BOOK_LEARNING_MODE)
        *best_move = select_random_move(movelist);

    return dabs(best);

}

double find_worst_stats(CGame * game, uint16_t * wins, uint16_t * losses, uint16_t * draws) {
    uint16_t movelist[11];
    uint16_t qualitylist[11];
    uint32_t learnlist[11];

    if(probe_book(game->zobrist_key, movelist, qualitylist, learnlist) == BOGUS_MOVE)
        return -2;

    double test, worst = 2;
    uint16_t unused;

    for(uint16_t i = 0; movelist[i]; i++) {

        game->make_move(movelist[i]);
        test = find_best_stats(game, BOOK_BEST_ONLY, &unused, wins, losses, draws);
        game->unmake_move();

        if(test < worst) worst = test;
    }

    if(worst > 1) return -2;
    return worst;
}