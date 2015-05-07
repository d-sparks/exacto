#pragma once
#include <vector>
#include "bb.cpp"
#include "inlines.h"
#include "masks.cpp"

using namespace std;

#define BISHOP_MAGIC_BITS 9
#define BISHOP_SHIFT 55 // 64-BISHOP_MAGIC_BITS
#define EXP2_BISHOP_MAGIC_BITS 512
#define ROOK_MAGIC_BITS 12
#define ROOK_SHIFT 52 // 64-ROOK_MAGIC_BITS
#define EXP2_ROOK_MAGIC_BITS 4096

namespace magics {

    // 9 bit bishop magics (generated with exacto 0.e)
    BB BISHOP_MOVES[64][EXP2_BISHOP_MAGIC_BITS];
    BB const BISHOP_MAGICS[64] = {
        0x0150510001100a407, 0x01020040820240400, 0x04801400808800208, 0x020469c0108004001,
        0x00002390084002941, 0x011a120201808041c, 0x00185000b00900080, 0x00801801080984001,
        0x00040022a84420201, 0x01000120010304202, 0x01140004a52008080, 0x00860042030281010,
        0x00000405c10000001, 0x00300c03022b10214, 0x001212941ac002000, 0x006031210a00a4084,
        0x08000500282300410, 0x08009002800602044, 0x02004000085000600, 0x0002060a8f0050004,
        0x04410482200500003, 0x0000082090020d000, 0x00a404006a0500100, 0x02011022040048020,
        0x00484012041002680, 0x00100808020040100, 0x0c0050040388080c0, 0x00020280201004008,
        0x00001001041024008, 0x04058820036220020, 0x0600a011022003000, 0x01380380800c12040,
        0x0a002824040a10420, 0x0a02c305108880052, 0x00000061000080042, 0x03200020080080080,
        0x00c2004040000c030, 0x014010020c0000920, 0x00020840301011082, 0x0000a400682204252,
        0x0500008010a942080, 0x08004086880320100, 0x034110a6010a00200, 0x00000086400e20080,
        0x00808400403000818, 0x01330d02010100020, 0x0021002490030001a, 0x00009c1800802a102,
        0x0cd02280040a10040, 0x02488420820100000, 0x02e11118022010041, 0x0000001400400a000,
        0x00838010440308019, 0x00080014304002124, 0x00002204848008002, 0x02008452801010100,
        0x02040400482200120, 0x000000a000a208a98, 0x00860244212005060, 0x08400200058410080,
        0x00010011201002824, 0x00004100041080108, 0x04050210a29080040, 0x00000508204090680
    };

    // 11 bit rook magics (generated with exaxcto 0.e)
    BB ROOK_MOVES[64][EXP2_ROOK_MAGIC_BITS];
    BB const ROOK_MAGICS[64]   = {
        0x00a8002c000108020, 0x00100084000802070, 0x08180048020009000, 0x00102002110000800,
        0x00100023302040800, 0x0220012000884b108, 0x02010158412000040, 0x00880004024800100,
        0x01a41800010400020, 0x000000ba00500c800, 0x000002000c8120005, 0x0000200080a002840,
        0x0002e00600800900c, 0x00408004808022204, 0x08002440a24040082, 0x00019400244008020,
        0x01003428000108024, 0x081020108003c0100, 0x00042800820100100, 0x06c00204200030040,
        0x04082000811a40100, 0x04820848820008040, 0x00920100a00004008, 0x02081820000241041,
        0x09009214104008000, 0x00000214016004090, 0x00804108040080041, 0x01204024010010440,
        0x00828040080020800, 0x00d02040028040486, 0x0018004c020800100, 0x08000040860044080,
        0x08030021008600021, 0x00000080488100404, 0x00040420110880008, 0x01401401023020580,
        0x02041000605000410, 0x03c08400c041000b3, 0x08400120090c02485, 0x000010200804002a0,
        0x00006100a21402000, 0x000a2108008085008, 0x01826080004001000, 0x0120040a42002000a,
        0x0000c004128000888, 0x000000080041004c0, 0x08140184000442c80, 0x04000010298420004,
        0x00100403000a51008, 0x008888040900c6020, 0x00068000a00200420, 0x0b0300a0084400200,
        0x00200c02400020040, 0x00008030004220508, 0x000a04a0001000480, 0x00000020020008810,
        0x000011b0080002241, 0x0d004284000104901, 0x000d808c200104282, 0x00420201a90000805,
        0x00100900500480001, 0x0040004980a440011, 0x00110048420520401, 0x0800011040280c422
    };

    // The magic formula, helper for bishops
    BB hashBishop(BB bb, ind square) {
        return (bb*BISHOP_MAGICS[square]) >> BISHOP_SHIFT;
    }

    // The magic formula, helper for rooks
    BB hashRook(BB bb, ind square) {
        return (bb*ROOK_MAGICS[square]) >> ROOK_SHIFT;
    }

    // bishopMoves gives the moves for a bishop given an occupancy bitboard and a square
    BB bishopMoves(ind square, BB occupancy) {
        occupancy &= masks::BISHOP_MASKS[square];
        return BISHOP_MOVES[square][hashBishop(occupancy, square)];
    }

    // rookMoves gives the moves for a bishop given an occupancy bitboard and a square
    BB rookMoves(ind square, BB occupancy) {
        occupancy = occupancy & masks::ROOK_MASKS[square];
        return ROOK_MOVES[square][hashRook(occupancy, square)];
    }

    // generateSubsets finds all sub-bitstrings of a given bitstring. It is used to generate occupancy
    // bitboards for magics. It works by going through indices x = 0, ..., 2^(popcount(b)) - 1,
    // associating each such x to a sub-bitstring of b by shifting the bits of x to the indexes of the
    // nonzero bits of b.
    void generateSubsets(BB b, vector<BB> * subsets) {
        ind pop = popcount(b);

        ind indices[pop];

        for(ind j = 0; b; j++) {
            ind i = bitscan(b);
            indices[j] = i;
            b &= ~exp_2(i);
        }

        for(BB x = 0; x < exp_2(pop); x++) {
            BB subset = 0;
            for(ind i = 0; i < pop; i++) {
                if(x & exp_2(i)) {
                    subset |= exp_2(indices[i]);
                }
            }
            subsets->push_back(subset);
        }
    }

    // Increments square by a delta and returns true if that square is still on the board.
    bool wrapIter(int *square, int delta) {
        *square += delta;
        // Check for overflowing on the top or bottom of the board
        bool wrapped = *square < 0 || *square >= 64;
        // Check for overflowing on the right
        wrapped |= (*square % 8 == 0) && ((16+delta) % 8 == 1);
        // Check for overflowing on the left
        wrapped |= (*square % 8 == 7) && ((16+delta) % 8 == 7);
        return !wrapped;
    }

    // Generate the moves for a given occupancy bitboard, square and set of directions.
    BB generateMovesFromOccupancy(ind square, BB occupancy, BB mask, int directionDeltas[4]) {
        BB moveBoard = 0;
        for(ind direction = 0; direction < 4; direction++) {
            int delta = directionDeltas[direction];
            int nextSquare = square;
            while(wrapIter(&nextSquare, delta)) {
                BB nextMove = exp_2(nextSquare);
                moveBoard |= nextMove;
                if((occupancy & nextMove) != 0 || (mask & nextMove) == 0) {
                    break;
                }
            }
        }
        return moveBoard;
    }

    // Populate the bishop move table for a certain square and a certain number of bits.
    void populateBishopTable(ind square) {
        BB mask = masks::BISHOP_MASKS[square];
        vector<BB> occupancyBoards;
        generateSubsets(mask, &occupancyBoards);
        for(int i = 0; i < occupancyBoards.size(); i++) {
            BB occupancy = occupancyBoards.at(i);
            int directionDeltas[4] = { 7, 9, -7, -9 };
            BB moveBoard = generateMovesFromOccupancy(square, occupancy, mask, directionDeltas);
            BISHOP_MOVES[square][hashBishop(occupancy, square)] = moveBoard;
        }
    }

    // Populate the rook move table for a certain square and a certain number of bits.
    void populateRookTable(ind square) {
        BB mask = masks::ROOK_MASKS[square];
        vector<BB> occupancyBoards;
        generateSubsets(mask, &occupancyBoards);
        for(int i = 0; i < occupancyBoards.size(); i++) {
            BB occupancy = occupancyBoards.at(i);
            int directionDeltas[4] = { 1, 8, -1, -8 };
            BB moveBoard = generateMovesFromOccupancy(square, occupancy, mask, directionDeltas);
            ROOK_MOVES[square][hashRook(occupancy, square)] = moveBoard;
        }
    }

    // Generate bishop move table for all squares.
    void populateBishopTables() {
        for(ind square = 0; square < 63; square++) {
            populateBishopTable(square);
        }
    }

    // Generate rook move table for all squares.
    void populateRookTables() {
        for(ind square = 0; square < 63; square++) {
            populateRookTable(square);
        }
    }

}
