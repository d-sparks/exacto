// game.cpp
// created: 9:55AM 1/2/2014
// author: Daniel Sparks
// license: See text/license.txt
// purpose: This is the gamestate class.

#include "exacto.h"
#include "inlines.h"

CGame::CGame(void) {
}

CGame::~CGame(void) {
}

/* =================================
    Make a move on the chess board.
   ================================= */

void CGame::make_move(uint16_t move) {

    uint8_t         from_square         = move & MASK_FROM,
                    to_square           = (move & MASK_TO) >> 6,
                    special             = (move & MASK_SPECIAL) >> 12;
    uint8_t         attacker            = board[from_square],
                    defender            = board[to_square];

    /*  Gamestate history stuff.  The zobrist key is stored at the end.  This creates a weird effect where
        there is no move stored in the 0 slot fo the history array, but the zobrist key of the opening position
        is stored there.  Would like to fix that for organizational purposes, but I'm too afraid of breaking
        something that will be annoying to fix.     */

    move_num++;
    hist_50m[move_num] = hist_50m[move_num - 1] + 1;
    hist_moves[move_num] = move;
    hist_defender[move_num] = defender;
    hist_epc[move_num] = castling[1] | castling[0] | en_passant;

    /*  Move the attacking piece first, and change the color to move.       */

    pieces[wtm][attacker] &= ~exp_2(from_square);
    pieces[wtm][attacker] |= exp_2(to_square);
    pieces[wtm][ALL] &= ~exp_2(from_square);
    pieces[wtm][ALL] |= exp_2(to_square);
    zobrist_key ^= (zobrist_pieces[wtm][attacker][from_square] ^ zobrist_pieces[wtm][attacker][to_square]) ^ zobrist_wtm;

    board[to_square] = attacker;
    board[from_square] = 0;

    /*  Empty the current en passant square.    */

    if(en_passant) {
        zobrist_key ^= zobrist_en_passant[bitscan(en_passant) % 8];
        en_passant = 0;
    }

    /*  If a rook or king moved, must check if castling rights are changed, or, if a pawn moved, update the
        pawn hash zobrist key and reset the 50 move counter.  Also update incremental eval stuff.   */

    if(attacker <= BISHOP) {
        incremental_positional += PST[wtm][attacker][to_square] - PST[wtm][attacker][from_square];
        if(attacker == PAWN) {
            zobrist_key_pawns ^= zobrist_pieces[wtm][PAWN][from_square] ^ zobrist_pieces[wtm][PAWN][to_square];
            hist_50m[move_num] = 0;
        }
    } else if(attacker == ROOK) {
        if(wtm && (castling[WHITE] != 0)) {
            if(from_square == 0)            castling[WHITE] &= (uint64_t)0xfffffffffffffffd; // ~exp_2(1)
            else if(from_square == 7)       castling[WHITE] &= (uint64_t)0xffffffffffffffdf; // ~exp_2(5)
        } else if(btm && (castling[BLACK] != 0)) {
            if(castling[BLACK]) {
                if(from_square == 56)           castling[BLACK] &= (uint64_t)0xfdffffffffffffff; // ~exp_2(57)
                else if(from_square == 63)      castling[BLACK] &= (uint64_t)0xdfffffffffffffff; // ~exp_2(61)
            }
        }
    } else if(attacker == KING)
        castling[wtm] = 0;

    if(defender) {
        pieces[btm][defender] &= ~exp_2(to_square);
        pieces[btm][ALL] &= ~exp_2(to_square);
        zobrist_key ^= zobrist_pieces[btm][defender][to_square];
        hist_50m[move_num] = 0;

        material_balance -= piece_values[!wtm][defender];

        if(defender <= BISHOP)
            incremental_positional -= PST[!wtm][defender][to_square];

        if(defender == PAWN) {
            num_of_pawns--;
            zobrist_key_pawns ^= zobrist_pieces[btm][PAWN][to_square];
        } else {
            num_of_pieces--;

            /*  Capturing a rook prevents castling. */

            if(defender == ROOK) {
                if(wtm && (castling[BLACK] != 0)) {
                    if(to_square == 56)             castling[BLACK] &= (uint64_t)0xfdffffffffffffff; // ~exp_2(57)
                    else if(to_square == 63)        castling[BLACK] &= (uint64_t)0xdfffffffffffffff; // ~exp_2(61)
                } else if(btm && (castling[WHITE] != 0)) {
                    if(to_square == 0)              castling[WHITE] &= (uint64_t)0xfffffffffffffffd; // ~exp_2(1)
                    else if(to_square == 7)         castling[WHITE] &= (uint64_t)0xffffffffffffffdf; // ~exp_2(5)
                }
            }
        }
    }

    wtm = btm;
    btm = !btm;

    occupied = pieces[wtm][ALL] | pieces[btm][ALL];


    /*  Now look at the special move flag for things like en passant, castling and promotion.   */

    if(special) {
        switch(special) {
        case DOUBLE_PAWN_MOVE_W:    en_passant = two_to_the[to_square-8];
                                    zobrist_key ^= zobrist_en_passant[to_square % 8];               break;

        case DOUBLE_PAWN_MOVE_B:    en_passant = two_to_the[to_square+8];
                                    zobrist_key ^= zobrist_en_passant[to_square % 8];               break;

        case EN_PASSANT_CAP_W:      special = to_square-8;
                                    pieces[BLACK][PAWN] &= ~exp_2(special);
                                    pieces[BLACK][ALL] &= ~exp_2(special);
                                    occupied &= ~exp_2(special);
                                    board[special] = 0;
                                    incremental_positional -= PST[wtm][PAWN][special];
                                    material_balance -= piece_values[wtm][PAWN];
                                    num_of_pawns--;
                                    zobrist_key ^= zobrist_pieces[BLACK][PAWN][special];
                                    zobrist_key_pawns ^= zobrist_pieces[BLACK][PAWN][special];      break;

        case EN_PASSANT_CAP_B:      special = to_square+8;
                                    pieces[WHITE][PAWN] &= ~exp_2(special);
                                    pieces[WHITE][ALL] &= ~exp_2(special);
                                    occupied &= ~exp_2(special);
                                    board[special] = 0;
                                    incremental_positional -= PST[wtm][PAWN][special];
                                    material_balance -= piece_values[wtm][PAWN];
                                    num_of_pawns--;
                                    zobrist_key ^= zobrist_pieces[WHITE][PAWN][special];
                                    zobrist_key_pawns ^= zobrist_pieces[WHITE][PAWN][special];      break;

        case KINGSIDE_CASTLE_W:     castling[WHITE] = 0;
                                    pieces[WHITE][ROOK] &= (uint64_t)0xfffffffffffffffe;
                                    pieces[WHITE][ROOK] |= (uint64_t)0x0000000000000004;
                                    pieces[WHITE][ALL] &= (uint64_t)0xfffffffffffffffe;
                                    pieces[WHITE][ALL] |= (uint64_t)0x0000000000000004;
                                    occupied &= (uint64_t)0xfffffffffffffffe;
                                    occupied |= (uint64_t)0x0000000000000004;
                                    zobrist_key ^= zobrist_pieces[WHITE][ROOK][0] ^ zobrist_pieces[WHITE][ROOK][2];
                                    hist_50m[move_num] = 0;
                                    board[0] = 0;
                                    board[2] = ROOK;                                                break;

        case QUEENSIDE_CASTLE_W:    castling[WHITE] = 0;
                                    pieces[WHITE][ROOK] &= (uint64_t)0xffffffffffffff7f;
                                    pieces[WHITE][ROOK] |= (uint64_t)0x0000000000000010;
                                    pieces[WHITE][ALL] &= (uint64_t)0xffffffffffffff7f;
                                    pieces[WHITE][ALL] |= (uint64_t)0x0000000000000010;
                                    occupied &= (uint64_t)0xffffffffffffff7f;
                                    occupied |= (uint64_t)0x0000000000000010;
                                    zobrist_key ^= zobrist_pieces[WHITE][ROOK][7] ^ zobrist_pieces[WHITE][ROOK][4];
                                    hist_50m[move_num] = 0;
                                    board[7] = 0;
                                    board[4] = ROOK;                                                break;

        case KINGSIDE_CASTLE_B:     castling[BLACK] = 0;
                                    pieces[BLACK][ROOK] &= (uint64_t)0xfeffffffffffffff; //~exp_2(56);
                                    pieces[BLACK][ROOK] |= (uint64_t)0x0400000000000000; //exp_2(58);
                                    pieces[BLACK][ALL] &= (uint64_t)0xfeffffffffffffff;
                                    pieces[BLACK][ALL] |= (uint64_t)0x0400000000000000;
                                    occupied &= (uint64_t)0xfeffffffffffffff;
                                    occupied |= (uint64_t)0x0400000000000000;
                                    zobrist_key ^= zobrist_pieces[BLACK][ROOK][56] ^ zobrist_pieces[BLACK][ROOK][58];
                                    hist_50m[move_num] = 0;
                                    board[56] = 0;
                                    board[58] = ROOK;                                               break;

        case QUEENSIDE_CASTLE_B:    castling[BLACK] = 0;
                                    pieces[BLACK][ROOK] &= (uint64_t)0x7fffffffffffffff; //~exp_2(63);
                                    pieces[BLACK][ROOK] |= (uint64_t)0x1000000000000000; //exp_2(60);
                                    pieces[BLACK][ALL] &= (uint64_t)0x7fffffffffffffff;
                                    pieces[BLACK][ALL] |= (uint64_t)0x1000000000000000;
                                    occupied &= (uint64_t)0x7fffffffffffffff;
                                    occupied |= (uint64_t)0x1000000000000000;
                                    zobrist_key ^= zobrist_pieces[BLACK][ROOK][63] ^ zobrist_pieces[BLACK][ROOK][60];
                                    hist_50m[move_num] = 0;
                                    board[63] = 0;
                                    board[60] = ROOK;                                               break;

        case PROMOTE_QUEEN:         pieces[btm][PAWN] &= ~exp_2(to_square);
                                    pieces[btm][QUEEN] |= exp_2(to_square);
                                    board[to_square] = QUEEN;
                                    material_balance += piece_values[btm][QUEEN] - piece_values[btm][PAWN];
                                    incremental_positional -= PST[btm][PAWN][to_square];
                                    zobrist_key ^= zobrist_pieces[btm][PAWN][to_square] ^ zobrist_pieces[btm][QUEEN][to_square];
                                    zobrist_key_pawns ^= zobrist_pieces[btm][PAWN][to_square];
                                    num_of_pieces++;
                                    num_of_pawns--;                                                 break;

        case PROMOTE_ROOK:          pieces[btm][PAWN] &= ~exp_2(to_square);
                                    pieces[btm][ROOK] |= exp_2(to_square);
                                    board[to_square] = ROOK;
                                    material_balance += piece_values[btm][ROOK] - piece_values[btm][PAWN];
                                    incremental_positional -= PST[btm][PAWN][to_square];
                                    zobrist_key ^= zobrist_pieces[btm][PAWN][to_square] ^ zobrist_pieces[btm][ROOK][to_square];
                                    zobrist_key_pawns ^= zobrist_pieces[btm][PAWN][to_square];
                                    num_of_pieces++;
                                    num_of_pawns--;                                                 break;

        case PROMOTE_BISHOP:        pieces[btm][PAWN] &= ~exp_2(to_square);
                                    pieces[btm][BISHOP] |= exp_2(to_square);
                                    board[to_square] = BISHOP;
                                    material_balance += piece_values[btm][BISHOP] - piece_values[btm][PAWN];
                                    incremental_positional -= PST[btm][PAWN][to_square] - PST[btm][BISHOP][to_square];
                                    zobrist_key ^= zobrist_pieces[btm][1][to_square] ^ zobrist_pieces[btm][3][to_square];
                                    zobrist_key_pawns ^= zobrist_pieces[btm][1][to_square];
                                    num_of_pieces++;
                                    num_of_pawns--;                                                 break;

        case PROMOTE_KNIGHT:        pieces[btm][PAWN] &= ~exp_2(to_square);
                                    pieces[btm][KNIGHT] |= exp_2(to_square);
                                    board[to_square] = KNIGHT;
                                    material_balance += piece_values[btm][KNIGHT] - piece_values[btm][PAWN];
                                    incremental_positional -= PST[btm][PAWN][to_square] - PST[btm][KNIGHT][to_square];
                                    zobrist_key ^= zobrist_pieces[btm][PAWN][to_square] ^ zobrist_pieces[btm][KNIGHT][to_square];
                                    zobrist_key_pawns ^= zobrist_pieces[btm][PAWN][to_square];
                                    num_of_pieces++;
                                    num_of_pawns--;                                                  break;
        }
    }

    empty = ~occupied;
    ht_threefold[zobrist_key >> (64 - 14)]++;
    hist_zobrist[move_num] = zobrist_key;

}


/* ======================================================================================
    Unmake move is the counterpart to make_move, undoing everything that make_move does.
   ====================================================================================== */

void CGame::unmake_move() {

    uint8_t         from_square         = hist_moves[move_num] & MASK_FROM,
                    to_square           = (hist_moves[move_num] & MASK_TO) >> 6,
                    special             = (hist_moves[move_num] & MASK_SPECIAL) >> 12;
    uint8_t         attacker            = board[to_square],
                    defender            = hist_defender[move_num];

    ht_threefold[zobrist_key >> (64 - 14)]--;

    wtm = btm;
    btm = !btm;

    pieces[wtm][attacker] &= ~exp_2(to_square);
    pieces[wtm][attacker] |= exp_2(from_square);
    pieces[wtm][ALL] &= ~exp_2(to_square);
    pieces[wtm][ALL] |= exp_2(from_square);

    board[from_square] = attacker;
    board[to_square] = defender;

    if(attacker <= BISHOP) {
        incremental_positional -= PST[wtm][attacker][to_square] - PST[wtm][attacker][from_square];
        if(attacker == PAWN)
            zobrist_key_pawns ^= zobrist_pieces[WHITE][PAWN][from_square] ^ zobrist_pieces[WHITE][PAWN][to_square];
    }

    if(defender) {
        pieces[btm][defender] |= exp_2(to_square);
        pieces[btm][ALL] |= exp_2(to_square);
        material_balance += piece_values[btm][defender];
        if(defender <= BISHOP)
            incremental_positional += PST[btm][defender][to_square];
        if(defender == PAWN) {
            num_of_pawns++;
            zobrist_key_pawns ^= zobrist_pieces[btm][PAWN][to_square];
        } else
            num_of_pieces++;
    }

    occupied = pieces[wtm][ALL] | pieces[btm][ALL];


    /*  Special move flags. */

    if(special) {
        switch(special) {
        case PROMOTE_BISHOP:
        case PROMOTE_KNIGHT:        incremental_positional -= PST[wtm][attacker][from_square];
        case PROMOTE_QUEEN:
        case PROMOTE_ROOK:          pieces[wtm][attacker] &= ~exp_2(from_square);
                                    pieces[wtm][PAWN] |= exp_2(from_square);
                                    board[from_square] = PAWN;
                                    material_balance -= piece_values[wtm][attacker] - piece_values[wtm][PAWN];
                                    incremental_positional += PST[wtm][PAWN][from_square];
                                    zobrist_key_pawns ^= zobrist_pieces[wtm][PAWN][from_square];
                                    num_of_pieces--;
                                    num_of_pawns++;                                                                                 break;

            break;

        case EN_PASSANT_CAP_W:      special = to_square-8;
                                    pieces[BLACK][PAWN] |= exp_2(special);
                                    pieces[BLACK][ALL] |= exp_2(special);
                                    occupied |= exp_2(special);
                                    board[special] = PAWN;
                                    material_balance += piece_values[btm][PAWN];
                                    incremental_positional += PST[btm][PAWN][special];
                                    num_of_pawns++;
                                    zobrist_key_pawns ^= zobrist_pieces[BLACK][PAWN][special];                                      break;

        case EN_PASSANT_CAP_B:      special = to_square+8;
                                    pieces[WHITE][PAWN] |= exp_2(special);
                                    pieces[WHITE][ALL] |= exp_2(special);
                                    occupied |= exp_2(special);
                                    board[special] = PAWN;
                                    material_balance += piece_values[btm][PAWN];
                                    incremental_positional += PST[btm][PAWN][special];
                                    num_of_pawns++;
                                    zobrist_key_pawns ^= zobrist_pieces[WHITE][PAWN][special];                                      break;

        case KINGSIDE_CASTLE_W:     pieces[WHITE][ROOK] |= (uint64_t)0x0000000000000001; //exp_2(0);
                                    pieces[WHITE][ROOK] &= (uint64_t)0xfffffffffffffffb; //~exp_2(2);
                                    pieces[WHITE][ALL] |= (uint64_t)0x0000000000000001;
                                    pieces[WHITE][ALL] &= (uint64_t)0xfffffffffffffffb;
                                    occupied |= (uint64_t)0x0000000000000001;
                                    occupied &= (uint64_t)0xfffffffffffffffb;
                                    board[0] = ROOK;
                                    board[2] = 0;                                                                                   break;

        case QUEENSIDE_CASTLE_W:    pieces[WHITE][ROOK] |= (uint64_t)0x0000000000000080; //exp_2(7);
                                    pieces[WHITE][ROOK] &= (uint64_t)0xffffffffffffffef; //~exp_2(4);
                                    pieces[WHITE][ALL] |= (uint64_t)0x0000000000000080;
                                    pieces[WHITE][ALL] &= (uint64_t)0xffffffffffffffef;
                                    occupied |= (uint64_t)0x0000000000000080;
                                    occupied &= (uint64_t)0xffffffffffffffef;
                                    board[7] = ROOK;
                                    board[4] = 0;                                                                                   break;

        case KINGSIDE_CASTLE_B:     pieces[BLACK][ROOK] |= (uint64_t)0x0100000000000000; //exp_2(56);
                                    pieces[BLACK][ROOK] &= (uint64_t)0xfbffffffffffffff; //~exp_2(58);
                                    pieces[BLACK][ALL] |= (uint64_t)0x0100000000000000;
                                    pieces[BLACK][ALL] &= (uint64_t)0xfbffffffffffffff;
                                    occupied |= (uint64_t)0x0100000000000000;
                                    occupied &= (uint64_t)0xfbffffffffffffff;
                                    board[56] = ROOK;
                                    board[58] = 0;                                                                                  break;

        case QUEENSIDE_CASTLE_B:    pieces[BLACK][ROOK] |= (uint64_t)0x8000000000000000; //exp_2(63);
                                    pieces[BLACK][ROOK] &= (uint64_t)0xefffffffffffffff; //~exp_2(60);
                                    pieces[BLACK][ALL] |= (uint64_t)0x8000000000000000;
                                    pieces[BLACK][ALL] &= (uint64_t)0xefffffffffffffff;
                                    occupied |= (uint64_t)0x8000000000000000;
                                    occupied &= (uint64_t)0xefffffffffffffff;
                                    board[63] = ROOK;
                                    board[60] = 0;                                                                                  break;
        }
    }

    empty = ~occupied;

    en_passant = hist_epc[move_num] & (mask_rank[2] | mask_rank[5]);
    castling[WHITE] = hist_epc[move_num] & mask_rank[0];
    castling[BLACK] = hist_epc[move_num] & mask_rank[7];
    move_num--;

    zobrist_key = hist_zobrist[move_num];

}

// =========================================================================================
//  Make and unmake null insert and undo the null move for the null move pruning technique.
// =========================================================================================

void CGame::make_null() {
    move_num++;
    wtm = btm;
    btm = !btm;
    zobrist_key ^= zobrist_wtm;

    hist_epc[move_num] = en_passant;
    hist_moves[move_num] = NULL_MOVE;
    hist_defender[move_num] = 0;
    hist_50m[move_num] = hist_50m[move_num];
    hist_zobrist[move_num] = zobrist_key;

    if(en_passant) {
        zobrist_key ^= zobrist_en_passant[bitscan(en_passant) % 8];
        en_passant = 0;
    }
}

void CGame::unmake_null() {
    en_passant = hist_epc[move_num];
    wtm = btm;
    btm = !btm;
    move_num--;
    zobrist_key = hist_zobrist[move_num];
}

/* ======================================================================
Sets up a position from FEN input.  Also accepts "Default","",""....
======================================================================  */

void CGame::set_board(string brd, string clr, string cstl, string ep, string hm, string fm) {
    string          temp;
    int             i, j;


    /*  Clear the board.    */

    for(i = 0; i <= 1; i++) {
        for(j = 0; j <= 6; j++) pieces[i][j] = 0;
        castling[i] = 0;
    }

    for(i = 0; i <= 63; i++) board[i] = 0;

    en_passant = 0; move_num = 0;

    if(brd == "Default") {
        set_board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR", "w", "KQkq", "-", "0", "0");
        return;
    }

    /*  Set the position from FEN input.    */

    for(unsigned int x = 0, y = 63; x < brd.length(); x++, y--) {
        temp = brd;
        temp = temp.substr(x, 1);
        if(atoi(temp.c_str()) == 0) {
            if(temp == "/")
                y++;
            else {
                if(temp == "p") { pieces[BLACK][PAWN] |= two_to_the[y]; board[y] = 1; }
                if(temp == "P") { pieces[WHITE][PAWN] |= two_to_the[y]; board[y] = 1; }
                if(temp == "n") { pieces[BLACK][KNIGHT] |= two_to_the[y]; board[y] = 2; }
                if(temp == "N") { pieces[WHITE][KNIGHT] |= two_to_the[y]; board[y] = 2; }
                if(temp == "b") { pieces[BLACK][BISHOP] |= two_to_the[y]; board[y] = 3; }
                if(temp == "B") { pieces[WHITE][BISHOP] |= two_to_the[y]; board[y] = 3; }
                if(temp == "r") { pieces[BLACK][ROOK] |= two_to_the[y]; board[y] = 4; }
                if(temp == "R") { pieces[WHITE][ROOK] |= two_to_the[y]; board[y] = 4; }
                if(temp == "q") { pieces[BLACK][QUEEN] |= two_to_the[y]; board[y] = 5; }
                if(temp == "Q") { pieces[WHITE][QUEEN] |= two_to_the[y]; board[y] = 5; }
                if(temp == "k") { pieces[BLACK][KING] |= two_to_the[y]; board[y] = 6; }
                if(temp == "K") { pieces[WHITE][KING] |= two_to_the[y]; board[y] = 6; }
            }
        } else
            y -= (atoi(temp.c_str()) - 1);
    }

    if(clr == "w") wtm = true;
    else wtm = false;

    btm = !wtm;

    castling[WHITE] = 0;    /* Castling */
    castling[BLACK] = 0;
    for(unsigned int x = 0; x < cstl.length(); x++) {
        temp = cstl;
        temp = temp.substr(x, 1);
        if(temp == "K") castling[WHITE] |= two_to_the[1];
        if(temp == "Q") castling[WHITE] |= two_to_the[5];
        if(temp == "k") castling[BLACK] |= two_to_the[57];
        if(temp == "q") castling[BLACK] |= two_to_the[61];
    }

    if(ep == "-")           /* En passant */
        en_passant = 0;
    else {
        int r, f;
        if(ep.substr(0, 1) == "a") r = 7;   if(ep.substr(0, 1) == "b") r = 6;
        if(ep.substr(0, 1) == "c") r = 5;   if(ep.substr(0, 1) == "d") r = 4;
        if(ep.substr(0, 1) == "e") r = 3;   if(ep.substr(0, 1) == "f") r = 2;
        if(ep.substr(0, 1) == "g") r = 1;   if(ep.substr(0, 1) == "h") r = 0;
        temp = ep.substr(1, 1);
        f = atoi(temp.c_str()) - 1;
        en_passant = two_to_the[(8 * f) + r];
    }

    if(fm != "-") i = atoi(fm.c_str());     /* Full moves */
    else i = 0;
    move_num = i;

    if(hm != "-") i = atoi(hm.c_str());     /* Half moves / move number */
    else i = 0;
    hist_50m[move_num] = i;


    for(i = 0; i <= 1; i++) for(j = 1; j <= 6; j++) pieces[i][0] |= pieces[i][j];

    occupied = pieces[BLACK][ALL] | pieces[WHITE][ALL];
    empty = ~occupied;


    /*  Evaluation related stuff.   */

    material_balance = 0;
    incremental_positional = 0;

    num_of_pawns = 0;               num_of_pieces = 0;

    for(i = 0; i <= 63; i++) {
        if(board[i] == 0) continue;
        else if(board[i] == PAWN) num_of_pawns++;
        else if(board[i] != KING) num_of_pieces++;
        if(board[i] <= BISHOP) {
            incremental_positional += PST[(pieces[WHITE][ALL] >> i) & 1][board[i]][i];
            incremental_positional += PST[(pieces[WHITE][ALL] >> i) & 1][board[i]][i];
        }
        material_balance += piece_values[(pieces[WHITE][ALL] >> i) & 1][board[i]];
    }

    /*  Reset hash table, history and draw detection stuff. */

    get_zobrist_keys();

    //move_num = 0;

    for(i = 0; i <= 16383; i++)
        ht_threefold[i] = 0;

    hist_50m[0] = 0;

    hist_zobrist[0] = zobrist_key;
    ht_threefold[zobrist_key >> (64 - 14)]++;

}

/* ====================================================================================================
    GET_PREV_MOVE returns the last played move in a given gamestate.  This is used primarily to avoid
    performing two null moves in a row.  See CEngine::pvs and CEngine::alphabeta.
   ==================================================================================================== */

uint16_t CGame::get_prev_move() {
    return hist_moves[move_num];
}
