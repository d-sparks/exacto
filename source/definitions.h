// definitions.h
// created: 1:41PM, 2/10/2014
// author: Daniel Sparks
// license: See text/license.txt
// purpose: Program-wide definitions.

#ifndef _definitions_h
#define _definitions_h


//  PREPROC OPT (EVAL)

#define         PAWN_VAL                    100
#define         KNIGHT_VAL                  316
#define         BISHOP_VAL                  300
#define         ROOK_VAL                    500
#define         QUEEN_VAL                   970

#define         _PAWN_STRUCTURE
#define             _PAWN_CHAIN             3
#define             _PAWN_BACKWARD          8
#define             _PAWN_ISOLATED          15
#define             _PAWN_PASSED            6
#define             _PAWN_DOUBLED           7

#define         _BISHOP_PAIR                50
#define         _ON_THE_MOVE                10

#define         _KING_SAFETY
#define             _NO_GOOD_CASTLE         12
#define             _NEAR_WEAK_FILE         8
#define             _NEAR_OPEN_FILE         12

#define         _ROOK_PLACEMENT
#define             _ROOK_OPEN_FILE         15
#define             _ROOK_AIMED_AT_WEAK     5
#define             _ROOK_ON_7TH            5

#define         _BISHOP_PLACEMENT
#define             _BAD_BISHOP             15

//#define           _PIECE_OBSTRUCTING_PAWN     5   // Doubled if the pawn is passed (not implemented)

#define         _KNIGHT_PLACEMENT
#define             _KNIGHT_OUTPOST         7
//#define               _PAWN_RESTRICTING       15 // Not implemented
#define         _PAWN_PLACEMENT
#define         _QUEEN_PLACEMENT

#define         _KING_BETWEEN_ROOKS         9
#define         _BISHOP_KNIGHT_DIFF

#define         _ATTACK_ZONES               10
#define             _ATTACKERS_DEFENDERS    50

#define         _UNDEFENDED_MINOR           7
#define         _CONNECTED_ROOKS            7
//#define           _KING_PRESSURE              7
//#define           _2MOVE_ATTACKS              4


//  PREPROC OPT (SEARCH)

#define         _ASPIRATION_WINDOW
#define         _KILLER_MOVES
#define         _HASH_PRUNING
#define         _RAZORING
#define         _FUTILITY_PRUNING
#define             _FUT_MAX_DEPTH          6
#define             _FUT_MARGIN             170
#define         _BETA_PRUNING
#define         _NULL_MOVE
#define         _LMR
#define         _IID

#define         _DELTA_PRUNING

#define         R                           2               // For null move (usually 2, 7, 9)
#define         R_ad_1                      6
#define         R_ad_2                      8

#define         LMR_FULL_MOVES              4               // Late move reductions settings (usually 4, 3)
#define         LMR_MIN_DEPTH               2

#define         FP_MARGIN                   250             // Futility pruning margins (usually 350, 575, 1050)
#define         PFNFP_MARGIN                460
//#define       RAZOR_MARGIN                75

#define         MAX_CHECK_EXTENSIONS        100             // Comment out to disable
#define         MAX_RECAP_EXTENSIONS        4
#define         MAX_PASSED_PAWN_EXTENSIONS  12


//  PREPROC OPT (OTHER)
#define         _HIDE_EARLY_POST
#define         _BOOK_MODE                  2
#define             _BOOK_AVERAGING                         // Good until book win ratio data is substantial (book move val >= it's overall statistics)
#define             _BOOK_LEARNING                          // If disabled, the book will not update the win/loss statistics
#define             BOOK_BEST_ONLY          0
#define             BOOK_GOOD_ONLY          1
#define             BOOK_LEARNING_MODE      2
#define             BOOK_DISABLED           3


//  THREADING OPTIONS
#define         CPUS                        4               // Multithreading
#define         MASTER                      0


//  BASIC GAME DEFINITONS

#define         WHITE                       1               // Standard defines
#define         BLACK                       0

#define         UNASSIGNED                  3
#define         SWITCHED                    2

#define         ALL                         0

#define         PAWN                        1
#define         KNIGHT                      2
#define         BISHOP                      3
#define         ROOK                        4
#define         QUEEN                       5
#define         KING                        6

#define         INFTY                       31000
#define         MATESCORE                   30000           // Scores for end of game states
#define         NEARMATE                    29000
#define         DRAWSCORE                   0


//  MAGICS
#define         BMS                         55              // These are the shifts necessary for the bishop and rook magics.
#define         RMS                         52


//  MASKS AND MOVES

#define         MASK_FROM                   63              // Masks and defines for moves
#define         MASK_TO                     4032
#define         MASK_SPECIAL                258048
#define         MASK_SQUARES                4095

#define         MASK_16BITS                 65535

#define         MASK_POP_COUNT              0xFFFF          // A mask for the table-lookup pop_count_large function (bitboards.cpp)

#define         HASH_FLAG                   3               // Masks for depth/flag/old combined hash entries
#define         HASH_DEPTH                  32764
#define         HASH_OLD                    32768

#define         BOGUS_MOVE                  0xf81           // Filler move types
#define         NULL_MOVE                   0xf82


//  TRANSPOSITION TABLE FLAGS

#define         HASH_EXACT                  3               // Flags for the hash table entries
#define         HASH_BETA                   2
#define         HASH_ALPHA                  1


//  MOVE SORTING VALUES

#define         SORT_PV                     31000
#define         SORT_SUGGESTION             29000
#define         SORT_GOOD_CAP               27000
#define         SORT_KILLER                 25000
#define         SORT_BAD_CAP                23000
#define         SORT_COUNTER                21000





//  SPECIAL MOVE FLAGS

#define         PROMOTE_QUEEN               1               // Move special flags
#define         PROMOTE_ROOK                2
#define         PROMOTE_BISHOP              3
#define         PROMOTE_KNIGHT              4
#define         DOUBLE_PAWN_MOVE_W          5
#define         DOUBLE_PAWN_MOVE_B          6
#define         EN_PASSANT_CAP_W            7
#define         EN_PASSANT_CAP_B            8
#define         KINGSIDE_CASTLE_W           9
#define         QUEENSIDE_CASTLE_W          10
#define         KINGSIDE_CASTLE_B           11
#define         QUEENSIDE_CASTLE_B          12
#define         REMOVE_KINGSIDE_CASTLING    13
#define         REMOVE_QUEENSIDE_CASTLING   14
#define         REMOVE_ALL_CASTLING         15


#endif