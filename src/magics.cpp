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
        0x150510001100a407, 0x1020040820240400, 0x4801400808800208, 0x20469c0108004001,
        0x2390084002941,    0x11a120201808041c, 0x185000b00900080,  0x801801080984001,
        0x40022a84420201,   0x1000120010304202, 0x1140004a52008080, 0x860042030281010,
        0x405c10000001,     0x300c03022b10214,  0x1212941ac002000,  0x6031210a00a4084,
        0x8000500282300410, 0x8009002800602044, 0x2004000085000600, 0x2060a8f0050004,
        0x4410482200500003, 0x82090020d000,     0xa404006a0500100,  0x2011022040048020,
        0x484012041002680,  0x100808020040100,  0xc0050040388080c0, 0x20280201004008,
        0x1001041024008,    0x4058820036220020, 0x600a011022003000, 0x1380380800c12040,
        0xa002824040a10420, 0xa02c305108880052, 0x61000080042,      0x3200020080080080,
        0xc2004040000c030,  0x14010020c0000920, 0x20840301011082,   0xa400682204252,
        0x500008010a942080, 0x8004086880320100, 0x34110a6010a00200, 0x86400e20080,
        0x808400403000818,  0x1330d02010100020, 0x21002490030001a,  0x9c1800802a102,
        0xcd02280040a10040, 0x2488420820100000, 0x2e11118022010041, 0x1400400a000,
        0x838010440308019,  0x80014304002124,   0x2204848008002,    0x2008452801010100,
        0x2040400482200120, 0xa000a208a98,      0x860244212005060,  0x8400200058410080,
        0x10011201002824,   0x4100041080108,    0x4050210a29080040, 0x508204090680
    };

    // 11 bit rook magics (generated with exaxcto 0.e)
    BB ROOK_MOVES[64][EXP2_ROOK_MAGIC_BITS];
    BB const ROOK_MAGICS[64]   = {
        0xa8002c000108020,  0x100084000802070,  0x8180048020009000, 0x102002110000800,
        0x100023302040800,  0x220012000884b108, 0x2010158412000040, 0x880004024800100,
        0x1a41800010400020, 0xba00500c800,      0x2000c8120005,     0x200080a002840,
        0x2e00600800900c,   0x408004808022204,  0x8002440a24040082, 0x19400244008020,
        0x1003428000108024, 0x81020108003c0100, 0x42800820100100,   0x6c00204200030040,
        0x4082000811a40100, 0x4820848820008040, 0x920100a00004008,  0x2081820000241041,
        0x9009214104008000, 0x214016004090,     0x804108040080041,  0x1204024010010440,
        0x828040080020800,  0xd02040028040486,  0x18004c020800100,  0x8000040860044080,
        0x8030021008600021, 0x80488100404,      0x40420110880008,   0x1401401023020580,
        0x2041000605000410, 0x3c08400c041000b3, 0x8400120090c02485, 0x10200804002a0,
        0x6100a21402000,    0xa2108008085008,   0x1826080004001000, 0x120040a42002000a,
        0xc004128000888,    0x80041004c0,       0x8140184000442c80, 0x4000010298420004,
        0x100403000a51008,  0x8888040900c6020,  0x68000a00200420,   0xb0300a0084400200,
        0x200c02400020040,  0x8030004220508,    0xa04a0001000480,   0x20020008810,
        0x11b0080002241,    0xd004284000104901, 0xd808c200104282,   0x420201a90000805,
        0x100900500480001,  0x40004980a440011,  0x110048420520401,  0x800011040280c422
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
        occupancy = occupancy & masks::BISHOP_MASKS[square];
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

    // Generate the moves for a given occupancy bitboard, square and set of directions.
    BB generateMovesFromOccupancy(ind square, BB occupancy, BB mask, int directionDeltas[4]) {
        BB moveBoard = 0;
        for(ind direction = 0; direction < 4; direction++) {
            int delta = directionDeltas[direction];
            for(ind j = square + delta; j < 64; j += delta) {
                BB nextMove = exp_2(j);
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
