// For moves

#define REGULAR_MOVE 0
#define PROMOTE_KNIGHT 2
#define PROMOTE_BISHOP 3
#define PROMOTE_ROOK 4
#define PROMOTE_QUEEN 5
#define DOUBLE_PAWN_MOVE_W 8
#define DOUBLE_PAWN_MOVE_B 9
#define EN_PASSANT_CAP 10
#define CASTLE 11
#define KING_MOVE 12
#define REMOVE_KINGSIDE_CASTLING 13
#define REMOVE_QUEENSIDE_CASTLING 14
#define REMOVE_ALL_CASTLING 15

#define KINGSIDE 0
#define QUEENSIDE 1

#define BOGUS_MOVE 0xfffffff

// For Board

#define WHITE 1
#define BLACK 0

#define ALL 0
#define NONE 0
#define PAWN 1
#define KNIGHT 2
#define BISHOP 3
#define ROOK 4
#define QUEEN 5
#define KING 6

// for evaluate

#define PAWN_VAL 100
#define BISHOP_VAL 300
#define KNIGHT_VAL 300
#define ROOK_VAL 500
#define QUEEN_VAL 950
#define DRAWSCORE 0
#define MATESCORE 10000
#define INFNTY 20000

// for hash

#define HASH_EXACT 11
#define HASH_BETA 10
#define HASH_ALPHA 9
#define HASH_EXACT_OLD 3
#define HASH_BETA_OLD 2
#define HASH_ALPHA_OLD 1
#define HASH_MISS 0
#define MARK_AS_OLD 247  // 11110111

// pieces

#define WHITE 1
#define BLACK 0

#define ALL 0
#define NONE 0
#define PAWN 1
#define KNIGHT 2
#define BISHOP 3
#define ROOK 4
#define QUEEN 5
#define KING 6

// For magics

#define BISHOP_MAGIC_BITS 9
#define BISHOP_SHIFT 55  // 64-BISHOP_MAGIC_BITS
#define EXP2_BISHOP_MAGIC_BITS 512
#define ROOK_MAGIC_BITS 12
#define ROOK_SHIFT 52  // 64-ROOK_MAGIC_BITS
#define EXP2_ROOK_MAGIC_BITS 4096

