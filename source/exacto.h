// exacto.h
// created: 11:11 PM 12/9/2013
// author: Daniel Sparks
// license: See text/license.txt
// purpose: Primary header file: function prototypes, externs and includes.

#ifndef _exacto_h
#define _exacto_h

#include "definitions.h"
#include "includes.h"
#include "engine.h"
#include "game.h"
#include "hash.h"

// Tune 'em Dan-o

extern int16_t RAZOR_MARGIN;
extern uint8_t RAZOR_MAX_DEPTH;

/* ---------------------
    Function prototypes
   --------------------- */

// tune.cpp
void            run_suite(CEngine * engine, uint16_t seconds_per_position);
void            crude_tune(CEngine * engine_1, CEngine * engine_2, int16_t MPS, int16_t base, int16_t increment, string from_file);
int             play_self(CEngine * engine_1, CEngine * engine_2, int16_t MPS, int16_t base, int16_t increment, /*int16_t C[2][4][3]*/ unsigned int C[2]);
void            parse_tuning_file(uint32_t * k, uint32_t * i, uint32_t * j, int score[30], string filename);


// debug.cpp
void            print_bitboard              (uint64_t bb);
void            make_bitboard_file          ();
void            make_bitboard_file_2        ();

// bitboards.cpp
void            initialize                  ();

// book.cpp
void            parse_book                  (string filename);
void            write_book                  (string filename);
void            new_book_entry              (CGame * game, uint16_t move);
void            delete_book_entries         (uint64_t key);
bool            is_book                     (bool post);
void            delete_book_learning_data   ();
uint16_t        probe_book                  (uint64_t key, uint16_t move_array[256], uint16_t quality_array[256], uint32_t learn_array[256]);
void            set_stats                   (uint64_t key, uint16_t move, uint16_t wins, uint16_t losses, uint16_t draws);
void            print_book_moves            (CGame * game);
void            update_book_learning        (CGame * game, CEngine * engine, uint8_t winner);
double          find_best_stats             (CGame * game, uint8_t random_mode, uint16_t * best_move, uint16_t * wins, uint16_t * losses, uint16_t * draws);
double          find_worst_stats            (CGame * game, uint16_t * wins, uint16_t * losses, uint16_t * draws);

// exacto.cpp
void            fill_PST                    (int16_t C[7][3]);

// go.cpp
void            go                          ();
void            reset_engine                ();
void            print_engine_stats          ();

// io.cpp
void            print_algebra               (uint16_t square);
string          get_algebra                 (uint16_t square);
void            print_move                  (uint16_t move);
uint8_t         terminal_width              ();
void            print_move_history          ();
string          int_to_string               (int in);
int             string_to_int               (string strin);
double          dabs                        (double x);

// magics.cpp
uint64_t        find_bishop_magic           (int square, int bits);
uint64_t        find_rook_magic             (int square, int bits);
void            find_bishop_magics          (int bits);
void            find_rook_magics            (int bits);

// movegen.cpp
uint16_t        encode_move                 (uint16_t source, uint16_t dest, uint16_t special);
uint16_t        extract_source              (uint16_t move);
uint16_t        extract_dest                (uint16_t move);
uint16_t        extract_special             (uint16_t move);


/* ------------------
    Global variables
   ------------------ */

// bitboards.cpp
extern  const   uint64_t                    two_to_the[64];
extern  const   uint64_t                    not_two_to_the[64];
extern  const   uint64_t                    all_set;
extern  const   uint64_t                    mask_rank[8];
extern  const   uint64_t                    not_mask_rank[8];
extern  const   uint64_t                    not_mask_file[8];
extern  const   uint64_t                    mask_file[8];
extern  const   uint64_t                    mask_file_rv[8];
extern  const   uint64_t                    knight_moves[64];
extern  const   uint64_t                    king_moves[64];
extern  const   uint64_t                    bishop_masks[64];
extern  const   uint64_t                    rook_masks[64];
extern  const   uint64_t                    mask_prior_ranks[64];
extern  const   uint64_t                    mask_latter_ranks[64];
extern  const   uint64_t                    light_dark_squares[2];
extern  const   uint64_t                    mask_ep_ranks[2];
extern  const   uint8_t                     promoting_rank[2];
extern  const   uint8_t                     adjacent_files[8];
extern  const   uint64_t                    attack_zones[5][64];

extern          uint64_t                    corresponding_files[256];
extern          uint64_t                    interceding_squares[64][64];
extern          uint64_t                    opposite_masks[64][64];

// bitscan.cpp
extern          uint8_t                     pc_table[65536];

// book.cpp
extern          uint32_t                    book_entries;
extern          uint64_t                    * book_position;
extern          uint16_t                    * book_move;
extern          uint16_t                    * book_quality;
extern          uint32_t                    * book_learn;

// evaluate.cpp
extern          int16_t                     PST[2][7][64];
extern          int16_t                     piece_values[2][7];
extern  const   int16_t                     king_placement_early[64];
extern  const   int16_t                     king_placement_late[64];

// hash.cpp

extern          int                         ht_value_pawns[131072];
extern          uint64_t                    ht_lock_pawns[131072];
extern          uint8_t                     ht_open_files[2][131072];
extern          uint8_t                     ht_passed_pawns[2][131072];
extern          uint8_t                     ht_weak_pawns[2][131072];

extern          uint8_t                     ht_threefold[16384];

extern  const   uint64_t                    zobrist_pieces[2][7][64];
extern  const   uint64_t                    zobrist_en_passant[8];
extern  const   uint64_t                    zobrist_castling[4];
extern  const   uint64_t                    zobrist_wtm;

// io.cpp

uint16_t        cin_to_move                 (CGame * game);

// movegen.cpp
extern          uint64_t                    rook_moves[64][4096];
extern          uint64_t                    bishop_moves[64][512];
extern          uint64_t                    pawn_checks[2][64];

// magics.cpp
extern  const   uint64_t                    rook_magics[64];                // Magics
extern  const   uint64_t                    bishop_magics[64];


#endif
