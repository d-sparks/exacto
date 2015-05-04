#pragma once
#include "bb.h"

using namespace std;

namespace masks {

    BB promoRank(bool color) {
        return color? (BB)0x00FF000000000000 : (BB)0x000000000000FF00;
    }

    BB const RANK[8] = {
        0x00000000000000FF, 0x000000000000FF00, 0x0000000000FF0000, 0x00000000FF000000,
        0x000000FF00000000, 0x0000FF0000000000, 0x00FF000000000000, 0xFF00000000000000
    };

    BB const FILE[8] = {
        0x8080808080808080, 0x4040404040404040, 0x2020202020202020, 0x1010101010101010,
        0x0808080808080808, 0x0404040404040404, 0x0202020202020202, 0x0101010101010101
    };


    // Knight moves[i]  0 0 0 0 0 0 0 0     King moves[i]  0 0 0 0 0 0 0 0
    //                  0 0 0 1 0 1 0 0                    0 0 1 1 1 0 0 0
    //                  0 0 1 0 0 0 1 0                    0 0 1 i 1 0 0 0
    //                  0 0 0 0 i 0 0 0                    0 0 1 1 1 0 0 0
    //                  0 0 1 0 0 0 1 0                    0 0 0 0 0 0 0 0
    //                  0 0 0 1 0 1 0 0                    0 0 0 0 0 0 0 0
    //                  0 0 0 0 0 0 0 0                    0 0 0 0 0 0 0 0
    //                  0 0 0 0 0 0 0 0                    0 0 0 0 0 0 0 0

    BB const KNIGHT_MOVES[64] = {
        0x0000000000020400, 0x0000000000050800, 0x00000000000a1100, 0x0000000000142200,
        0x0000000000284400, 0x0000000000508800, 0x0000000000a01000, 0x0000000000402000,
        0x0000000002040004, 0x0000000005080008, 0x000000000a110011, 0x0000000014220022,
        0x0000000028440044, 0x0000000050880088, 0x00000000a0100010, 0x0000000040200020,
        0x0000000204000402, 0x0000000508000805, 0x0000000a1100110a, 0x0000001422002214,
        0x0000002844004428, 0x0000005088008850, 0x000000a0100010a0, 0x0000004020002040,
        0x0000020400040200, 0x0000050800080500, 0x00000a1100110a00, 0x0000142200221400,
        0x0000284400442800, 0x0000508800885000, 0x0000a0100010a000, 0x0000402000204000,
        0x0002040004020000, 0x0005080008050000, 0x000a1100110a0000, 0x0014220022140000,
        0x0028440044280000, 0x0050880088500000, 0x00a0100010a00000, 0x0040200020400000,
        0x0204000402000000, 0x0508000805000000, 0x0a1100110a000000, 0x1422002214000000,
        0x2844004428000000, 0x5088008850000000, 0xa0100010a0000000, 0x4020002040000000,
        0x0400040200000000, 0x0800080500000000, 0x1100110a00000000, 0x2200221400000000,
        0x4400442800000000, 0x8800885000000000, 0x100010a000000000, 0x2000204000000000,
        0x0004020000000000, 0x0008050000000000, 0x00110a0000000000, 0x0022140000000000,
        0x0044280000000000, 0x0088500000000000, 0x0010a00000000000, 0x0020400000000000
    };

    BB const KING_MOVES[64] = {
        0x0000000000000302, 0x0000000000000705, 0x0000000000000e0a, 0x0000000000001c14,
        0x0000000000003828, 0x0000000000007050, 0x000000000000e0a0, 0x000000000000c040,
        0x0000000000030203, 0x0000000000070507, 0x00000000000e0a0e, 0x00000000001c141c,
        0x0000000000382838, 0x0000000000705070, 0x0000000000e0a0e0, 0x0000000000c040c0,
        0x0000000003020300, 0x0000000007050700, 0x000000000e0a0e00, 0x000000001c141c00,
        0x0000000038283800, 0x0000000070507000, 0x00000000e0a0e000, 0x00000000c040c000,
        0x0000000302030000, 0x0000000705070000, 0x0000000e0a0e0000, 0x0000001c141c0000,
        0x0000003828380000, 0x0000007050700000, 0x000000e0a0e00000, 0x000000c040c00000,
        0x0000030203000000, 0x0000070507000000, 0x00000e0a0e000000, 0x00001c141c000000,
        0x0000382838000000, 0x0000705070000000, 0x0000e0a0e0000000, 0x0000c040c0000000,
        0x0003020300000000, 0x0007050700000000, 0x000e0a0e00000000, 0x001c141c00000000,
        0x0038283800000000, 0x0070507000000000, 0x00e0a0e000000000, 0x00c040c000000000,
        0x0302030000000000, 0x0705070000000000, 0x0e0a0e0000000000, 0x1c141c0000000000,
        0x3828380000000000, 0x7050700000000000, 0xe0a0e00000000000, 0xc040c00000000000,
        0x0203000000000000, 0x0507000000000000, 0x0a0e000000000000, 0x141c000000000000,
        0x2838000000000000, 0x5070000000000000, 0xa0e0000000000000, 0x40c0000000000000
    };
};
