// hash.cpp
// created: 11:39PM 12/19/13
// author: Daniel Sparks
// license: See text/license.txt
// purpose: The main transposition table.

#include "exacto.h"
#include "inlines.h"

uint64_t            ht_lock_pawns[131072];              // === The pawn positional evaluation hash table ===
int                 ht_value_pawns[131072];             // 2^17 entries, 18 bytes per entry ~ 40mB?
uint8_t             ht_open_files[2][131072];
uint8_t             ht_passed_pawns[2][131072];
uint8_t             ht_weak_pawns[2][131072];
#define             PHS                                 47

uint8_t             ht_threefold[16384];                // 2^14 entries, 1 byte per entry = 14 kB

uint64_t            zobrist_key;                        // The zobrist keys which reflect the gamestate.
uint64_t            zobrist_key_pawns;
/*uint64_t  const   zobrist_wtm;                        // I declared these below the functions (scroll down) for readability.
                    zobrist_castling[4];
                    zobrist_en_passant[8];
                    zobrist_pieces[2][7][64]; */

CHash::CHash(void) {

    set_dimension(128);
    delete PV_lock;
    delete PV_move;

    PV_lock = new uint64_t[PV_size];
    PV_move = new uint16_t[PV_size];

}

CHash::~CHash(void) {

    delete lock;
    delete sugg;
    delete old_depth_flag;
    delete value;

    delete PV_lock;
    delete PV_move;

}

// ===================================================================================================================================
//  Dimension hash table and set hash table are functions which are typically called once to initialize, but not during a game.
//  Dimension hash table sets the size of the hash table (in MB of RAM), set hash table clears the table and resets the zobrist keys.
// ===================================================================================================================================

void CHash::set_dimension(unsigned int size_in_mb) {
    size = (1024 * 1024 * size_in_mb) / 14;

    delete lock;
    delete sugg;
    delete old_depth_flag;
    delete value;

    lock = new uint64_t[size];
    sugg = new uint16_t[size];
    old_depth_flag = new uint16_t[size];
    value = new int16_t[size];

    for(uint64_t i = 0; i < size; i++) {
        lock[i] = 0;
        old_depth_flag[i] = 0;
    }
}

void CHash::clear_table() {
    unsigned int i;

    for(i = 0; i < size; i++) {                                                     // Clear the transposition table
        lock[i] = 0;
        old_depth_flag[i] = 0;
    }

    for(i = 0; i <= 32767; i++) {                                                   // Clear the pawn structure evaluation hash table
        ht_lock_pawns[i] = 0;
        PV_lock[i] = 0;
    }
}

void CGame::get_zobrist_keys() {
    zobrist_key = 0;                                                                // Reset the zobrist keys
    zobrist_key_pawns = 0;

    if(wtm) zobrist_key ^= zobrist_wtm;                                             // The rest of this function sets both zobrist keys to reflect the current gamestate

    for(uint8_t i = 0; i <= 63; i++) {
        if(board[i]) zobrist_key ^= zobrist_pieces[(pieces[WHITE][ALL] & exp_2(i)) != 0][board[i]][i];
        if(board[i] == PAWN) zobrist_key_pawns ^= zobrist_pieces[(pieces[WHITE][ALL] & exp_2(i)) != 0][PAWN][i];
    }

    if(en_passant) zobrist_key ^= zobrist_en_passant[bitscan(en_passant) % 8];

    if(castling[1] & exp_2(1)) zobrist_key ^= zobrist_castling[0];
    if(castling[1] & exp_2(5)) zobrist_key ^= zobrist_castling[1];
    if(castling[0] & exp_2(57)) zobrist_key ^= zobrist_castling[2];
    if(castling[0] & exp_2(61)) zobrist_key ^= zobrist_castling[3];
}




// ========================================================================================================================================
//  New hash (entry), probe hash (table), (get) hash key, and should prune (based on table) all are used for the main transposition table.
// ========================================================================================================================================

void CHash::record(uint64_t key, uint16_t suggestion, uint8_t depth, uint8_t flag, int16_t score) {
    uint64_t hashed_lock = key % size;

    if(((old_depth_flag[hashed_lock] & HASH_OLD) != 0) || (flag > (old_depth_flag[hashed_lock] & HASH_FLAG))
        || ((flag == (old_depth_flag[hashed_lock] & HASH_FLAG)) && (depth >= ((old_depth_flag[hashed_lock] & HASH_DEPTH) >> 2)))) {

        lock[hashed_lock] = key;                                // Replace hash table entry
        sugg[hashed_lock] = suggestion;
        value[hashed_lock] = score;
        old_depth_flag[hashed_lock] = flag | (depth << 2);
    } else
        old_depth_flag[hashed_lock] |= HASH_OLD;                // Mark the entry for replacement when the next type 2 collision occurs

    if(flag == HASH_EXACT) {
        PV_lock[key % PV_size] = key;
        PV_move[key % PV_size] = suggestion;
    }

}

uint8_t CHash::probe(uint64_t key, uint8_t depth) {
    uint64_t hashed_key = key % size;
    if((key == lock[hashed_key]) && (depth <= ((old_depth_flag[hashed_key] & HASH_DEPTH) >> 2))) {
        return old_depth_flag[hashed_key] & HASH_FLAG;
    }
    return 0;
}

void CHash::delete_entry(uint64_t key) {
    old_depth_flag[key % size] = 0;
    lock[key % size] = 0;
    PV_lock[key % PV_size] = 0;
}

// ====================================================================================================
//  CLEAR REPITITONS performs a minimax search to depth "depth," clearing all hash table entries such
//  that a descendant node (within the specified minimax tree) represents a repeated position in the
//  sense of threefold repititions.
// ====================================================================================================

bool CHash::clear_repititions(CGame * game, uint8_t depth) {

    bool found_draw = game->repeated(game->move_num);
    if(depth != 0) {
        uint16_t movelist[256];
        game->generate_moves(movelist);
        for(uint8_t i = 0; movelist[i]; i++) {
            game->make_move(movelist[i]);
            found_draw |= clear_repititions(game, depth - 1);
            game->unmake_move();
        }
    }
    if(found_draw) delete_entry(game->zobrist_key);
    return found_draw;
}

// ======================================================================
//  Similar functions as above, but from the pawn evaluation hash table.
// ======================================================================

uint64_t CGame::hash_key_pawns() {
    return zobrist_key_pawns >> PHS;
}

void CGame::new_pawn_hash(int value) {
    uint64_t hashed_key = hash_key_pawns();

    ht_weak_pawns[BLACK][hashed_key] = weak_pawns[BLACK];
    ht_weak_pawns[WHITE][hashed_key] = weak_pawns[WHITE];
    ht_open_files[BLACK][hashed_key] = open_files[BLACK];
    ht_open_files[WHITE][hashed_key] = open_files[WHITE];
    ht_passed_pawns[BLACK][hashed_key] = passed_pawns[BLACK];
    ht_passed_pawns[WHITE][hashed_key] = passed_pawns[WHITE];

    ht_value_pawns[hashed_key] = value;
    ht_lock_pawns[hashed_key] = zobrist_key_pawns;
}

bool CGame::probe_pawn_hash() {
    if(ht_lock_pawns[hash_key_pawns()] == zobrist_key_pawns) return true;
    return false;
}


// =============================================================================================================================
//  Constant zobrist keys used are here at the bottom so that the functions are easy to get to.  Keys are in hex for concision.
// =============================================================================================================================


uint64_t    const   zobrist_wtm                 =       0x6ed49c46f19bb8ea;
uint64_t    const   zobrist_castling[4]         =   {   0x2ec4712b0bfdfcd6, 0x6c6f5767fff27330, 0x71083b972d80c0c,  0x8d8325e82c4fdcdc};
uint64_t    const   zobrist_en_passant[8]       =   {   0x2ec4712b0bfdfcd6, 0x6c6f5767fff27330, 0x71083b972d80c0c,  0x8d8325e82c4fdcdc,
                0xb47a658dad8e13a4, 0x88710bf005fda027, 0x69bd3edaf7111200, 0xdccdd0c65c810ff};
uint64_t    const   zobrist_pieces[2][7][64]    =   {{{ 0x64d79b552a559d7f, 0x44a572665a6ee240, 0xeb2bf6dc3d72135c, 0xe3836981f9f82ea0,
                0x43a38212350ee392, 0xce77502bffcacf8b, 0x5d8a82d90126f0e7, 0xc0510c6f402c1e3c, 0x48d895bf8b69f77b, 0x8d9fbb371f1de07f,
                0x1126b97be8c91ce2, 0xf05e1c9dc2674be2, 0xe4d5327a12874c1e, 0x7c1951ea43a7500d, 0xbba2bbfbecbc239a, 0xc5704350b17f0215,
                0x823a67c5f88337e7, 0xd9bf140bfeb4c1a9, 0x9fbe3cfcd1f08059, 0xdc29309412e352b9, 0x5a0ff7908b1b3c57, 0x46f39cb43b126c55,
                0x9648168491f3b126, 0xdd3e72538fd39a1c, 0xd65a3663fc9b0898, 0x421ee7823c2af2eb, 0xcba3a4b69b6ed152, 0x348399b7d2b8428,
                0xbdb750bf00c34a38, 0xcf36d95eae514f52, 0x7b9231d5308d7534, 0xb225e28cfc5aa663, 0xa833f6d5c72448a4, 0xdaa565f5815de899,
                0x4b051d1e4cc78eb8, 0x97fcd1b4d342e575, 0xef6a48be001729c7, 0x3982f1fa31afeab8, 0xfdc570ba2fe979fb, 0xb57697121dfdfe93,
                0x96524e209b767c29, 0x55aad0ebca994043, 0xb22687b88856b63e, 0xb313b667a4d999d6, 0x7c7fa1bd6fd7dea,  0xee9f4c15c57e92a,
                0xc5fb71b8f4bf5f56, 0xa251f93a4b335492, 0xb9bad7f9e5b07bef, 0x62fc9ac35ccde7aa, 0x933792382b0218a3, 0x7d497d2f7a15eaf8,
                0xb2f0624214f522a2, 0xd9895bbb810ec3d6, 0x22d91b683f251121, 0xc8fe9a347247affd, 0x3462898a2ae7b001, 0x468bc3a10a34890c,
                0x84ff6ce56552b185, 0xed95ff232c511188, 0x4869be47a8137c83, 0x934606951e6fcd81, 0x1ab5e8e453bde710, 0x6386b61b30fa1157},
                                                    {   0x3d7ed9954afafa75, 0x63d444cd9661fd5,  0xca96991f92d0fe14, 0x16196e9b8dabb301,
                0x86127b848ccc08e1, 0xa71538a4eb0548d8, 0x1850c6666f78ac55, 0x4e81b24346ee26ca, 0xbdae3de19a449a4b, 0xa4d79f9ef9ee72bd,
                0xb0a2edbf2358e268, 0x62cc18f6c732d9d,  0xa0d910f06702acd4, 0x1490cfe087834a71, 0xe6be7d4742f01152, 0x4feb4fecc0cc23be,
                0xf3102a3e33bdcb88, 0x2264ab364ae4f3a9, 0x67acc8a5b422242,  0xfe532bc4d90abd0,  0xc59dfd658a765f52, 0x7a993f2ac89ebec,
                0x4b382a7583b9f799, 0xae0933b09f8bc00d, 0xb4b1e63954a6f4a,  0x36670728154e61b3, 0x13b19b6f1a6e2cbc, 0xcbd6731ace2bc1ae,
                0x508e20c8f67ab323, 0x38bce480fcee979a, 0x3cabfe02edc2eb27, 0xbc4adf2c91ac18b4, 0x4bc2bf21efe747b3, 0xe8f77dbf150a4452,
                0xf49bea0dce18fa8c, 0xa0dea65179b30f6b, 0xd809d9cd3219c864, 0xf1356c9e807111df, 0x6ba3f7af9f41ac90, 0x6ee77daa94553291,
                0xe5fa480a757345e9, 0xa3b9b87c2c2188a9, 0x7cbff889c544e627, 0xd4dd51e32f0457ad, 0xb439f6964d874b24, 0xda418091dcfa879b,
                0x8ae0ab55cb4d2025, 0x3eedb3eeefc3ce20, 0xb850cb0beb05f339, 0x66041c070a933877, 0x11e5ca39acc788e5, 0x2978cb7dd2f3b76d,
                0xdb2df796407915f0, 0xf10bc233e60d4ffe, 0x65dd301ea6a1e5c0, 0x4904c2e4cea0607,  0xce3d581ab9c4d867, 0xe37605bc15f2524c,
                0x2dea1c383dade1a1, 0xabfde08e43e84393, 0xde4196cbeae9f552, 0xeb31f35efa2f770a, 0x3a8bba219be51e0d, 0xc28a696c81808848},
                                                    {   0x5d39ed705d278264, 0xf3f25e4d062ac13d, 0x698854ee3abade90, 0x5325e26c2d5d4fe0,
                0x31f136789b4e9309, 0xc9e39db8406800e,  0xdf57629355052112, 0x31645c4f6694074,  0x6b3c1226d6c90174, 0x3e69f6254e760602,
                0x3cc6290f6c21ec83, 0xe0b271ee622c4eea, 0xfc413953e0b07715, 0xc4f9a759f313cc8b, 0xf75d8bac5ea9292e, 0x2c7c312463e635a0,
                0x6d9ff6b4828fc5a3, 0xf3632b250fabcf1d, 0xa7b4677e430cab5b, 0xa8d4b6b8d1d2d0d4, 0x6656325153ee8873, 0x226e142bca89cfa0,
                0x73a474af4d194f98, 0x877737c54d46b4e6, 0x7af0fd2266f4654e, 0x63e52dbed86ce2fe, 0x52bbe15db6363ac7, 0x75ad0a5887a5f552,
                0xecb318b77395e13b, 0x43dec48b0c539f48, 0xa90cae0dbcac879f, 0x848f7bab3fcad4a4, 0x488f530592db2c52, 0x91b7df8aff93285c,
                0x77fa013a568df553, 0xe4f3cd15575233eb, 0x3d11bfea8027bc86, 0x1c452d3ee6284ec,  0xfda2bdf77d66a691, 0xf4360dccc5c1da01,
                0x69895eef72b4b0a4, 0xe82e5ea2fafd12f3, 0x1d4a668f32aa2544, 0x2ddbb7814c837556, 0xee7d11acdf5eb62e, 0xc9c91d2e7e5768d6,
                0x2504009691e013de, 0x468f5799a38d1b83, 0x70e52bcabb35cc43, 0xd8c0d02bcf55c620, 0x372259ba7b887425, 0x723a6fafcad70103,
                0x8c3919c90f8797be, 0xc8106891649cd651, 0x4d6a70e7e2680724, 0x27860e363d3f6649, 0xb98a20d51b5f8d41, 0x7e91a07829feef8c,
                0x27cd3dd3e619bebf, 0x8ce625f390ce06e5, 0xe3d675f2e48310f9, 0x4000d5f00540ec8b, 0xed4ce3172243a6eb, 0x6d138662f96d96b7},
                                                    {   0xb33783c186f091a,  0x69723968b83f188d, 0xb3d96a0eaba10ef7, 0x691b8f18602cdc18,
                0xce688d839783dcb3, 0xc57fdb0fc8f5b373, 0xe25bb2d74061d6a8, 0x8bfe8851cd7dca38, 0x8c9a055f0aab3762, 0xd503631f6e6a8ae9,
                0x5d502cdb49a13d56, 0x23fc98d9734d7225, 0x357a2c3a432f7ec5, 0x241261d3218d3940, 0xc7ed41555056e9a0, 0xe0926f1212a6c02b,
                0x47c80400df432ff4, 0x7ad9dfedb53af59a, 0x2c1eebdfcdb2d876, 0x353350a26a90b4c,  0x254459ca217295d0, 0xd459ce57fcdbde85,
                0xaa787116f8ce3b46, 0x101cefe274101378, 0x70b7beea10653330, 0x92ac487ee62971ee, 0x8b96376546170220, 0x80fd00d5d419913d,
                0x17cde54538f64d21, 0xc7afc237b47b21c3, 0x50f42d6ce8af8a5e, 0xbb8560a62335007,  0x5090012c95321ec,  0x84b8c32bce05e09d,
                0x133edf99dd192b85, 0x9509fd4429d57227, 0x947324d3251ab37,  0xd77066c5d2abb9a0, 0xe238b1582bd6ef36, 0xf8e4aa3247a7c0a8,
                0x252d4a8de7884b94, 0x5f2c30420fa39382, 0x1cbab1c583932ac1, 0x9cea7e838f5b65ea, 0x3af890d53c54f9ee, 0x3b38a11c9f392e76,
                0x9ae580b5cafd651c, 0x59f602c1eb89ce03, 0xed211debc8bd8151, 0xa35ac34aab4d0f66, 0xd87158f6481ddece, 0x80d3eb220244b01,
                0xeb32d57d1967bbff, 0x960ddb4364dc4b01, 0x5f03118425461954, 0x291d7a7f273cadd6, 0xde0357b6bc8f93ec, 0x35dd2b081a15da06,
                0x560e099344647287, 0x5d56f990a5034ae2, 0xca2d0d74f5e00c9a, 0x64195edf2ff80a88, 0xe69a5d4c9e8c21ee, 0xa659fc69be304199},
                                                    {   0x2e8e95ef2ff101a0, 0x87bec846f184fd42, 0x98035eefa603acde, 0x8436815eb6a774aa,
                0x2762218e890c244d, 0x92df81b61c6a90cb, 0x107534d44986ee4b, 0xea9446a097bc9d3,  0xbb85888ea36eba19, 0x909896aa47565b13,
                0x5b0836338c825c0c, 0x4abaad22f76e447a, 0xb578bb8b2bf1de74, 0xdb2581c61c4c6797, 0xc1b1e97727a93d0,  0xce02a570683897b4,
                0x8d3b0551da547e02, 0x892ed14657a01f68, 0x7d3601c53e409bb6, 0x15356b22147a32e1, 0x3d1f80c96b650e3c, 0x356aad12acc81622,
                0x3cf8022a4335a71e, 0x91b872719d68532c, 0x54e35c5ed696b065, 0xa152bfbe0e2ff726, 0xd1ea0276d4647d3a, 0x3a7d685f6e15e352,
                0x14dadd45946ffd99, 0x674c45f7ebb0241a, 0x17dfd371614a4e1d, 0x909e3ff3a8b8943d, 0xed8691f7c4d427f8, 0x668a76619a458a02,
                0x6b77e28f2fa814c0, 0x79ffef210d236cbe, 0xa27fccd9109c798e, 0xf14b24f5307fc542, 0x49eb812fc5ffc7ee, 0xd9c45087006177a,
                0x1aadb5cc54e27f7c, 0x617e5901b928afc7, 0x95ca4d8efa61a441, 0x52a9e8a1f521bf85, 0x645c552b85e719fa, 0xc58ba1a6ef1381c0,
                0x21487093b18f9988, 0xc78753aaa9034d6,  0xdecbb892a2f18cfd, 0xe14e30a9f4a9d308, 0xd3d831388cdf4be,  0x4faa73d8169adf51,
                0x689458b0b0ea6dc6, 0x4b2a496f9456c6e7, 0xc6cd31b1f86f7c47, 0xf3d9fab243fba6b6, 0xaadb7adf927b05c5, 0xdf4af98a087fe1f7,
                0xf00f418ff9af5d6d, 0xd10ada444210befc, 0xf0bbe74363156bc1, 0x2b8748add02e253a, 0x4f8f0998ed2c46aa, 0x7608f0cc518a041f},
                                                    {   0x64416a9b3a64b225, 0x64fd17052de48dcf, 0xd8749e4ab0f9b10c, 0xdf2913b04a83b462,
                0x59e1495bf686e801, 0x403c4c4a9b841992, 0xc26c2975696e09dc, 0x62d908df94a151ac, 0x2b275bb6f5675e5e, 0xeb0cdc6f6ee91a5f,
                0x611e4a9647e8054f, 0x7fdbf11ea0c71c07, 0x640622acb9e5f364, 0x282e5dbb52a1044f, 0xb13fd6d90538b258, 0x61e4a930226999b1,
                0x9539b45475274946, 0xcf4596732ea965b8, 0xb62ece77d43dd196, 0x25139b8852f1a6e5, 0x1ae6eb4aae0ee51b, 0x1f80cea7246958df,
                0xca1a0ccd5b7fc2f,  0xfb50e530bcaf8489, 0x9fe975198d88b6f9, 0xb0c7277a2518b20,  0xf3b850a03246eea8, 0xc4f0bf98866a125,
                0x67663cb935c5a6cb, 0x69f58e493bccbe7a, 0x4db649eed01cd338, 0x1d7b81e2f3a04c62, 0xb8f52ab9a223e033, 0xd8a81aaa3ebbce0b,
                0xe56922f70fcc0e19, 0x47b3486bc079bae6, 0x5395e3caa0f73960, 0x2927dce4cb1b5118, 0x8dcc05ee81a21e1e, 0x404e536ae741c67f,
                0xf1fd1e1e10acbe35, 0x30df5df449d58c4f, 0x2a5fac260393a867, 0xb13e7f029c7e4dfa, 0xea3122e85b9b24df, 0xeb97884724b947ad,
                0xeba994eb09a3714b, 0xca47add2cb7dc629, 0x94169f059560e8da, 0xdbec79ec4f221bcc, 0x9186f3a9e15cbfe0, 0x585abeb1b6b78ea8,
                0xe3d2d26399bf15f7, 0xb7999d1ada312de5, 0xeef430ad13ea4981, 0x5916a178f478b212, 0xa3ab1d1433984411, 0xbddf29647e35cb63,
                0x394f09aef78c6567, 0x7092e2f4911bf285, 0x223081ca0140d981, 0x4552875dd9c46a22, 0xc188df6d142fe4c4, 0x9b61b7b87d51a09c},
                                                    {   0x2895e400acd3536f, 0x739116216197d5f4, 0xd10666ced29cac61, 0x322751ac02c9b8ad,
                0x783370250a468352, 0xae7e50a99293e6b,  0x1ac9bead3a924ce9, 0xcc0d190e53bcdc32, 0x84589a876aaa74ec, 0x87af2fc2103f4a30,
                0x9e59353cdb8e68cd, 0x606611760eeb3e50, 0x6f4608ae33850147, 0x664b75d69769f902, 0x3fe3dd0ba61083a4, 0xae40debfb6804a8a,
                0x65deeefcd983141d, 0xf40f2b72795b156e, 0x3a12e08c8bef3acc, 0xdab1fd9843fa2ee1, 0x910a983e3d9aea71, 0x723275e31f8a9b88,
                0xd1552fd2d224fef9, 0x6489db43b097c544, 0xaaf6a3b4fdb7fd38, 0x7faa74387aac9461, 0x239f5cfa0484deb5, 0x200eef436a1380ec,
                0x5ad210ccc8a747bf, 0x3e1d8a8f08f5b463, 0xed7afb033738e242, 0x3c5ec48ece7de00f, 0x860e2405876c96eb, 0x2007da5154fb8bb1,
                0x7583d136a49901db, 0xad8f18247d0e2389, 0x2a617a8156f88122, 0x144d2891cd5b12af, 0x58e79aa13f71d551, 0xb01181252249d043,
                0x851d12e0c0038833, 0x34b43c6a2fa78d30, 0xce402085dbbd7055, 0x808a347a777ba59,  0xd972d754baed3678, 0xb18df2650c4f04bc,
                0xc4d9bdf9e45db3d3, 0x62876e592f45bbc4, 0xdf04c646d12d012a, 0x99a8d99af9034d1,  0xa6956b8f5314be7e, 0x8ea7467bed38ee89,
                0x8888d4f6ca0c7256, 0x92bc91a06eb161bd, 0x6749a973a892bc9b, 0x7b5947f2f30ebc0b, 0x75c9f70aec90b254, 0xa873b101be5851d4,
                0x159fb03e4bfe43a,  0x4d3ed2856863a9e9, 0x657bdb9cc621ed44, 0xe9658157dd2f37ae, 0x4733a5d15c269a23, 0xd95886b1678b5d69}},
                                                    {{  0x3308e9133a950fe3, 0x939ed8b0d7e91f87, 0x666beee64002b6b0, 0xc8f129ec69ce7811,
                0xd57593c68ce93ea0, 0x2d6a3e66edcc862,  0xbe1d00d16a2271a6, 0x34fbeaf95e0c673f, 0x9845ab59483a0e86, 0x257d47d5abf88962,
                0x28af39f39319545b, 0xe3fce03abd8171ee, 0xa4c5f606dccc96f1, 0x4d414846267c4962, 0x6ccf77f81d9dbf70, 0x947bf43c729a71ec,
                0xfd656c39c4fa824c, 0x8f652cf2d1e04fd6, 0x8cb11929a65b6aee, 0x94948f16a8064da,  0x7434e703a4d03d5f, 0x9361d3f63af4aa35,
                0xa998c1eeec3fb422, 0x51eb94754b5992a2, 0x6e109c0347ef6979, 0xe3c9738d67c582e2, 0x9c735e3857ec57bf, 0xbe6415659e12c64b,
                0x73924584e31b9099, 0x8f676821e60b0945, 0x5614e3a695d5289d, 0x7ecd448787517eba, 0xc96db02038dbaf5b, 0x69299ed774fa6c8,
                0xb4ace5a8ea16ac8,  0xbf2f4f23a6c92295, 0x90bdc4f1e931656c, 0x7cd5b0b95ac34d3a, 0x2032bc59d3dc1710, 0x702c1a0cd5609379,
                0x609d33abc01ff3fe, 0x8ae5d8f283b2748b, 0x2cf3778fa7eaae1d, 0xe8a0d7b1919df9e3, 0xe487894f6d602a0b, 0x929858549609626a,
                0x46e540cd86bf46e7, 0xd1daf4382128d9ea, 0xc47239c06b22ef75, 0x8b7aad8ffea1b991, 0xd6c1d2e315273fa0, 0x2fda11cd74177e6b,
                0x333cb0a145919fd7, 0x5970b31a49f37b16, 0x7890bc68793bb959, 0x2a060f45a1719347, 0xeb298f0264bf379d, 0xd7c4fd7921707400,
                0x374635e7713ed165, 0xc60c008df0296d05, 0xbf13739a8d3c7dbb, 0xbfb945ef1cf94d1d, 0x75fe953c3a3a8315, 0x9f83064f4150c02},
                                                    {   0xfdeccc962bb135d,  0x36711226025fc6cb, 0x16feb2dace20c308, 0x9ce9c15bd5242467,
                0x35ecde5c52b0c98c, 0x1ae7b6b920043af1, 0x34e6363925d62112, 0x5d66ca5c18d5a80b, 0x5ff816fe6a223100, 0x91fac37e6cbb4458,
                0x6c90e5bfa0b2a9d4, 0x5d2a1a2eefe924dd, 0xd3d509a36578c39a, 0x8e77687bddf6b488, 0x7ab86dfd49d4339e, 0x555ed7bda840704f,
                0xd8064138f65af1cd, 0x3c9415f0b7450ef9, 0xa6b91ff74d0b802d, 0xfc96c81bfb14c918, 0x2f0e8e88f776ca27, 0x15ea3c4025c93e99,
                0x56de67619fea7de,  0xed0fb45ad15c0935, 0x1ed721157283fe1e, 0x94589e9d30995cee, 0x96df769b4ae8540d, 0x9b6aadfa3efe8d0e,
                0x7b5836f305833d07, 0xe5daab13a8932a2f, 0x790bbe1d6ea3dede, 0x2c9993b2217ea0a,  0xa6585e53f0311e26, 0xa39875667223b0b6,
                0x934cd074647249da, 0xd684edf63631f8f9, 0xc83386526aa3b0c7, 0x54f701829b2e145f, 0xe2bf92d18ac71abb, 0x54ba2ca884356722,
                0xf3288de7c561f3ca, 0x3bcaf08d4d23301a, 0x995840e581b88194, 0x1e19c39bced9d869, 0xcb3c2c41ab469347, 0xecbdca4d5f76d4fd,
                0xaa9d4bba20fb9bc6, 0x30669220cd59f98f, 0x143c5a8d4e1929d8, 0xfcf188664688147f, 0x7b9cd85ed4051926, 0x4d6591ee6ebe7a73,
                0xc1559f89c425e137, 0x6a73be02c541686a, 0x95b7c30c4f5789ab, 0xf8815daad6fb27b3, 0x7cb999c0f3b1536b, 0xf0d7900fa4185882,
                0x8ecf9d38f5960719, 0x28f301ce5bd874c9, 0x4d81efb6beecd5d6, 0xfeb4ebf0cbd7d2f,  0xdba27dd41d3fc6,   0xbdb228d75d4ddf9},
                                                    {   0xd73f25c2fea35778, 0x7198d43c47f5181d, 0x7c7f08f67d43aa1f, 0x809295235a8d3696,
                0x81d37dcc5fc38e50, 0x3193801040358044, 0x7c4cea8d88e91dcc, 0x92b3ed1ca5a401c4, 0xb138a63ad5c63c14, 0xb091ea0d3b54e439,
                0xe4c05112aa1964da, 0x67bd40915838cd,   0x44f4b410c12e393c, 0x4e8b9bab9061101c, 0xddae74466a59e7d,  0xcce3025dba8470e5,
                0x15604c919a3bdf5a, 0x935da2f50f7c4316, 0x4820c3a4e4250f04, 0xfbb87b249ece5b68, 0x375a505f4cf8669c, 0xe258652b08de45e4,
                0xd31f79521cbe1ec6, 0x93cd866539e8d727, 0x51145d7f77e6c7b,  0x73b74b73c397356e, 0x98b5daa7322091cf, 0xcd80816698167be8,
                0x9e04ca4c73e23f3e, 0xd81ca915e24bdf7,  0x4a02847e91d09f21, 0xef6275c810493843, 0xa46a04df823a8fe9, 0xda9b3c2c22d5bafe,
                0x4f83db8ab726c46d, 0xa1a77791aa147e91, 0x91e3f5c4a856c68f, 0x2c915961d1b01b51, 0x9f2020beab2a3672, 0x7d22bc573046d1ab,
                0x9d5db91865eba3e4, 0xb0a5df1993f20d55, 0x325ae7a51fce7829, 0xb9d5f004a9b80cb1, 0x1b12de2658afcd52, 0x27f1729ad04de43c,
                0xe63aadc510eb2569, 0xd01bf87a0138501d, 0x82ad2833f68a1c5b, 0xc35724f3f2ccad26, 0xea2a0470626aab48, 0xbb3fc752b7e9d4fc,
                0x172873e19c33aab3, 0xc273b2dbe43ec090, 0x5730668503316887, 0x83f21acfafe65803, 0xb8120b7139e28775, 0x48b2c19fd5da2579,
                0x7c8cff4e7d93184d, 0x20f04d0674531d71, 0x5699c26bedd71652, 0xc2bcda3389601d21, 0x32e3ebf1425541b4, 0x9177e1dd4273290c},
                                                    {   0xdbe096ac79ab4ef0, 0x97a38f031bc9eb67, 0x3895440bdd8056d8, 0x2ee8a162d1c5eb05,
                0xc242d7bfd3a69971, 0x95670f6fe0a1a98c, 0x712e9f09a1e58507, 0xab498c4bdf223d92, 0xc402410f3e87d00f, 0xfc7c24566847db51,
                0xe13a8c5b17ae03e2, 0x148892bf918f0a53, 0x26882fe9596e8ab3, 0xc908277b37d9d2a1, 0x3eb055676b286955, 0x5df2ff5a8fb59b6b,
                0xc8511d3ab2cc95ca, 0x93df1221fc50527e, 0x4dde46e20ef2c71a, 0xb5cacea8aa557b5c, 0x698aeaa59724526e, 0xa1bdf8dfe100b110,
                0x12247d69e5c63387, 0x9da9a79bb938313f, 0x73c2ed4697c2c85b, 0x9e32a10f930966ef, 0x54ee8a54801f632f, 0xedd37e4f17cfd901,
                0xd2fd686a7e24609c, 0xdafb8aeaaf76413d, 0xf49f537020fbf76b, 0x34ba6b96dd9b94fc, 0x88b364c0e7d8b22f, 0xc3ee4e14f745fc58,
                0x32040bf84e584dc4, 0xbf1dffc876ebdf1b, 0x330fb523fe3bc41b, 0xf6ef31cff4b85f1f, 0xec34f739c1d9654d, 0x2761039da775be0e,
                0xe0cea01967d0fe42, 0xb846ee50f9e312d9, 0x32ff9c7f8758d2d3, 0x768ce7e091ba309a, 0x987988dc9e99474,  0x33d322c44d7a7ed7,
                0x33b309935d0770ff, 0x1831338b0966007d, 0x9ee41363a94d0641, 0x24d9d69900525dc4, 0xf8f830af454f6021, 0x1ff223c81322850d,
                0xff8607df9e9bfc0f, 0x19bd5d660eb12bc8, 0x9ccb50522981df33, 0x4cba2f79612c6708, 0x9de4c5ffebd1c1b7, 0xb35459e445de1951,
                0x544eb8fc7c1c0fff, 0xefd4b75d98ad296d, 0x7f1d6768133b0249, 0x54a3fbeb0c2beff,  0xdbcf44bd7c579dfe, 0xc0d774e4cc4e1f1f},
                                                    {   0x7ebc509928829cad, 0x66c0ebad794a0be6, 0x8227d92e2711a4f7, 0xfc9fb46f4936a938,
                0xb687e05b958a19f2, 0xb1b401f451612acd, 0x1b70d2934040b01e, 0x5351daa2d6827d53, 0x6ef613fc46f89190, 0x2fcd717c4b199e2c,
                0xc1239a8e1d833c7a, 0x35350fb21f8e376a, 0x95f42b3b61c42009, 0x293cfd6035706f51, 0xea3da1a579896598, 0xf441e23178049686,
                0x6f05331416c720ab, 0x2128aa3f73feac2,  0x92d7699df9c7285b, 0xc381c2e9612dbae6, 0x87575ce7e80828e,  0x66ab25932158b3a2,
                0x171b8deebcb0c2fd, 0xf4e11e51b824b917, 0x5b30181ece9d2f3e, 0x9e557ffdd01dc1d4, 0x22958ba97d243088, 0x35b78e916dfafefb,
                0xa1160dd80ef56f49, 0xc65b8ce6e66253b9, 0x40ef00582a1d7c4c, 0x3f28bc225d60966b, 0x53c46a44708e7e51, 0x5ff53f13281368fb,
                0x7b2c54dbdd5d4518, 0xbceff8a26f232ee9, 0xdd8df03ca1a114a8, 0x6dd09022731adaff, 0x9931d85bfa25479e, 0x67f4e32d1caed4ec,
                0xb1c9529939519d0,  0x915ddd79d7999f87, 0x9a595d1898e346ff, 0xb231b2698f52d45f, 0x1f74d8a81ae2c80e, 0xaf7a903068785bcc,
                0x51c7aad6a43e96f0, 0x822045f177c464bb, 0x6211425ccb6fbc14, 0x91f1377aae2e5612, 0x93c7402528e85141, 0xca3c1b54e1e7eb0a,
                0xa9d448fff5994f7e, 0x1de61dead4513943, 0x3a6fb9cb65c0f37,  0xc5e52c312071eab3, 0xf790a2241a212c,   0xa645e773a8cf07f1,
                0x7d1e7fb7483782e7, 0x71c92b13cac89c37, 0x336e66a354b92f8,  0xcdd7c410ada29a7,  0x9437f0c3c72ac1a1, 0x5859f76051d5c4ee},
                                                    {   0x5c8d22b9affd13eb, 0x133157756e085e1c, 0x4391452085919ba2, 0xc0e743cbcfc56291,
                0xe5851178bd06cff1, 0x29f7de4e9c36e2e4, 0x707270a27263a6c7, 0x50636e2db1fdef3c, 0x9d2cd64a1a01a561, 0xe14a6efe30b786ba,
                0xfae6239ee7d5a23c, 0x67540268e8dc9c12, 0x87aa4d4d97aba11c, 0x5b928d229232bdb0, 0x8c5daf7a095361e2, 0x65f54f7eaf8db494,
                0x51abee6648c0d18e, 0xfc3bfeb8ce47162f, 0x60812532b8a7ed36, 0x3aaacf642345670c, 0xf84996eca123c7e2, 0xc6fa19628cc8942d,
                0xdceb3fa86051016f, 0xf2916e0b16e9a3bb, 0xaa184bb331be3b35, 0xcc4d3cc64bf6303f, 0x589a441bf0172ee3, 0x6fa4331ee13f8b62,
                0xf4084ac3c2982dbe, 0xd89f1ac143146803, 0xb47ffe1b40fe2a55, 0xe4b4e98154fb535e, 0x4bfe6422f609c9ce, 0xaaf264e4e316863f,
                0xbcf0b43e91961637, 0xd323790c89119095, 0xc79e31a944e8ed18, 0x9b0ccc94deb0f8d,  0x3101f950950142b,  0x55a9fd1993ab6415,
                0xba38633c0b8a99ff, 0x226664446ea3eb54, 0x7b3b328c5856fada, 0x2477b2dbedb799f7, 0x111277a528adc97b, 0xd2b5a86a1b347666,
                0x3e488bb9e5b34383, 0x850715412b21f27b, 0x49c978127cc4c60c, 0xe94a4cd8fe8043ef, 0x8ef2a0c474549011, 0x4c96e6baf3dcc9ef,
                0xcfbfc996a38eafb5, 0x36e043795394f0af, 0x3d99335db8de2d13, 0xff4d58e5a75b843d, 0xaa181b4cbbba8747, 0x757acec938c3f125,
                0x20314647aec1b84a, 0x94ea2963875fba0d, 0xec08f6eb603fcb6e, 0xe002cbe3934e4c50, 0x5c55b490c0a4fe29, 0xca30bc0914d8bc21},
                                                    {   0xc7c50493ade2aa9,  0x99cd0dc5c6c5833f, 0xc393993e629b9b77, 0xcfaed3c67a5df583,
                0xa3f00644070b24f9, 0x8b80f18d9a2ceee9, 0xb74e6bd6bbfcc21c, 0xc6c23096a2e865cc, 0x51629caa6f0984ba, 0x4eda22518bba8c3b,
                0x2b7e01584980556d, 0x33cbbc71fca3eaa9, 0xe143ea691114f6bb, 0x1dfb58a13f0fe253, 0x6ef57333610a40ff, 0x4b8e82f7aeb30d89,
                0x864b1e7b9a3349ab, 0x4b3e45eb1b574a54, 0x40d7438f070639e4, 0xc3ce4e5738f1ded3, 0x4541309fed7f51e,  0xd8ecf44129d7a65c,
                0xfde4d5d8e4d05a34, 0x5c663dbb8635ff62, 0xb5ce44112682acb5, 0xd36e624cd2a4644c, 0x15fedce11be53ebd, 0x1b4816379cb43870,
                0x98affc2599e804f3, 0x41725d1f25d42ebb, 0xc5be4a6e2dc6f846, 0x9f0f4bd12638a23e, 0x68acabab1090749b, 0x238969566771019,
                0x44f0a739e9cf670b, 0x1fa0c78184b80c67, 0xe7f9a3e707e015d1, 0xd3c0dff63a354258, 0x5c66e3d7e3cc4e2d, 0xf32f0a8bba369df0,
                0xd404f45c4e7a3c23, 0x2eefc0331f5aac94, 0xf1da3ed97fd923f4, 0x4d87e776eae2ee58, 0xfc8bc28e23860ea0, 0x748726cea1ef6871,
                0xe88782f1354db5d,  0xdb32cc4f6d3aaf68, 0xd9c9140f969882c9, 0x30bb9e3b670416e2, 0x1abec011044e4a9f, 0xe8c8ef02a5490504,
                0x4c7fe748a77bb455, 0x7b7456c97e4eee8e, 0xab43c59b8d95030,  0xbf0e732ada3ffa4c, 0xe4be330f678992c9, 0xe4b4ad6fc18611ae,
                0x93cb2ab6cf34f12,  0xcf8f302e1a87898,  0xee1f67ded33415db, 0x807f696060045fbb, 0x216e4a489097d0d4, 0xa0e12e87648964b}}};