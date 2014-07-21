// bitboards.cpp
// created: 12:59 AM 12/10/13
// author: Daniel Sparks
// license: See text/license.txt
// purpose: Contains basic bitboard operations/masks.

#include "exacto.h"
#include "inlines.h"

/* ----------
    Bitmasks
   ---------- */



    /* Two to the[11]   0 0 0 0 0 0 0 0     Not two to the[11]  1 1 1 1 1 1 1 1     All set     1 1 1 1 1 1 1 1
                        0 0 0 0 0 0 0 0                         1 1 1 1 1 1 1 1                 1 1 1 1 1 1 1 1
                        0 0 0 0 0 0 0 0                         1 1 1 1 1 1 1 1                 1 1 1 1 1 1 1 1
                        0 0 0 0 0 0 0 0                         1 1 1 1 1 1 1 1                 1 1 1 1 1 1 1 1
                        0 0 0 0 0 0 0 0                         1 1 1 1 1 1 1 1                 1 1 1 1 1 1 1 1
                        0 0 0 0 0 0 0 0                         1 1 1 1 1 1 1 1                 1 1 1 1 1 1 1 1
                        0 0 0 0 1 0 0 0                         1 1 1 1 0 1 1 1                 1 1 1 1 1 1 1 1
                        0 0 0 0 0 0 0 0                         1 1 1 1 1 1 1 1                 1 1 1 1 1 1 1 1  */

uint64_t    const   two_to_the[64]              =   {   0x0000000000000001, 0x0000000000000002, 0x0000000000000004, 0x0000000000000008,
                0x0000000000000010, 0x0000000000000020, 0x0000000000000040, 0x0000000000000080, 0x0000000000000100, 0x0000000000000200,
                0x0000000000000400, 0x0000000000000800, 0x0000000000001000, 0x0000000000002000, 0x0000000000004000, 0x0000000000008000,
                0x0000000000010000, 0x0000000000020000, 0x0000000000040000, 0x0000000000080000, 0x0000000000100000, 0x0000000000200000,
                0x0000000000400000, 0x0000000000800000, 0x0000000001000000, 0x0000000002000000, 0x0000000004000000, 0x0000000008000000,
                0x0000000010000000, 0x0000000020000000, 0x0000000040000000, 0x0000000080000000, 0x0000000100000000, 0x0000000200000000,
                0x0000000400000000, 0x0000000800000000, 0x0000001000000000, 0x0000002000000000, 0x0000004000000000, 0x0000008000000000,
                0x0000010000000000, 0x0000020000000000, 0x0000040000000000, 0x0000080000000000, 0x0000100000000000, 0x0000200000000000,
                0x0000400000000000, 0x0000800000000000, 0x0001000000000000, 0x0002000000000000, 0x0004000000000000, 0x0008000000000000,
                0x0010000000000000, 0x0020000000000000, 0x0040000000000000, 0x0080000000000000, 0x0100000000000000, 0x0200000000000000,
                0x0400000000000000, 0x0800000000000000, 0x1000000000000000, 0x2000000000000000, 0x4000000000000000, 0x8000000000000000};
uint64_t    const   not_two_to_the[64]          =   {   0xfffffffffffffffe, 0xfffffffffffffffd, 0xfffffffffffffffb, 0xfffffffffffffff7,
                0xffffffffffffffef, 0xffffffffffffffdf, 0xffffffffffffffbf, 0xffffffffffffff7f, 0xfffffffffffffeff, 0xfffffffffffffdff,
                0xfffffffffffffbff, 0xfffffffffffff7ff, 0xffffffffffffefff, 0xffffffffffffdfff, 0xffffffffffffbfff, 0xffffffffffff7fff,
                0xfffffffffffeffff, 0xfffffffffffdffff, 0xfffffffffffbffff, 0xfffffffffff7ffff, 0xffffffffffefffff, 0xffffffffffdfffff,
                0xffffffffffbfffff, 0xffffffffff7fffff, 0xfffffffffeffffff, 0xfffffffffdffffff, 0xfffffffffbffffff, 0xfffffffff7ffffff,
                0xffffffffefffffff, 0xffffffffdfffffff, 0xffffffffbfffffff, 0xffffffff7fffffff, 0xfffffffeffffffff, 0xfffffffdffffffff,
                0xfffffffbffffffff, 0xfffffff7ffffffff, 0xffffffefffffffff, 0xffffffdfffffffff, 0xffffffbfffffffff, 0xffffff7fffffffff,
                0xfffffeffffffffff, 0xfffffdffffffffff, 0xfffffbffffffffff, 0xfffff7ffffffffff, 0xffffefffffffffff, 0xffffdfffffffffff,
                0xffffbfffffffffff, 0xffff7fffffffffff, 0xfffeffffffffffff, 0xfffdffffffffffff, 0xfffbffffffffffff, 0xfff7ffffffffffff,
                0xffefffffffffffff, 0xffdfffffffffffff, 0xffbfffffffffffff, 0xff7fffffffffffff, 0xfeffffffffffffff, 0xfdffffffffffffff,
                0xfbffffffffffffff, 0xf7ffffffffffffff, 0xefffffffffffffff, 0xdfffffffffffffff, 0xbfffffffffffffff, 0x7fffffffffffffff};
uint64_t    const   all_set                     =       0xffffffffffffffff;



    /*  Mask rank[2]    0 0 0 0 0 0 0 0     Not mask rank[2]    1 1 1 1 1 1 1 1
                        0 0 0 0 0 0 0 0                         1 1 1 1 1 1 1 1
                        0 0 0 0 0 0 0 0                         1 1 1 1 1 1 1 1
                        0 0 0 0 0 0 0 0                         1 1 1 1 1 1 1 1
                        0 0 0 0 0 0 0 0                         1 1 1 1 1 1 1 1
                        1 1 1 1 1 1 1 1                         0 0 0 0 0 0 0 0
                        0 0 0 0 0 0 0 0                         1 1 1 1 1 1 1 1
                        0 0 0 0 0 0 0 0                         1 1 1 1 1 1 1 1 */

uint64_t    const   mask_rank[8]                =   {   0x00000000000000ff, 0x000000000000ff00, 0x0000000000ff0000, 0x00000000ff000000,
                0x000000ff00000000, 0x0000ff0000000000, 0x00ff000000000000, 0xff00000000000000};
uint64_t    const   not_mask_rank[8]            =   {   0xffffffffffffff00, 0xffffffffffff00ff, 0xffffffffff00ffff, 0xffffffff00ffffff,
                0xffffff00ffffffff, 0xffff00ffffffffff, 0xff00ffffffffffff, 0x00ffffffffffffff};



    /*  Mask prior[i]   0 0 0 0 0 0 0 0     Mask latter[i]  1 1 1 1 1 1 1 1
                        0 0 0 0 0 0 0 0                     1 1 1 1 1 1 1 1
                        0 0 0 0 0 0 0 0                     1 1 1 1 1 1 1 1
                        0 0 0 0 0 0 0 0                     1 1 1 1 1 1 1 1
                        0 0 0 0 0 0 0 0                     1 1 1 1 1 1 1 1
                        1 1 1 1 i 1 1 1                     1 1 1 1 i 1 1 1
                        1 1 1 1 1 1 1 1                     0 0 0 0 0 0 0 0
                        1 1 1 1 1 1 1 1                     0 0 0 0 0 0 0 0  */

uint64_t    const   mask_prior_ranks[64]        =   {   0x00000000000000ff, 0x00000000000000ff, 0x00000000000000ff, 0x00000000000000ff,
                0x00000000000000ff, 0x00000000000000ff, 0x00000000000000ff, 0x00000000000000ff, 0x000000000000ffff, 0x000000000000ffff,
                0x000000000000ffff, 0x000000000000ffff, 0x000000000000ffff, 0x000000000000ffff, 0x000000000000ffff, 0x000000000000ffff,
                0x0000000000ffffff, 0x0000000000ffffff, 0x0000000000ffffff, 0x0000000000ffffff, 0x0000000000ffffff, 0x0000000000ffffff,
                0x0000000000ffffff, 0x0000000000ffffff, 0x00000000ffffffff, 0x00000000ffffffff, 0x00000000ffffffff, 0x00000000ffffffff,
                0x00000000ffffffff, 0x00000000ffffffff, 0x00000000ffffffff, 0x00000000ffffffff, 0x000000ffffffffff, 0x000000ffffffffff,
                0x000000ffffffffff, 0x000000ffffffffff, 0x000000ffffffffff, 0x000000ffffffffff, 0x000000ffffffffff, 0x000000ffffffffff,
                0x0000ffffffffffff, 0x0000ffffffffffff, 0x0000ffffffffffff, 0x0000ffffffffffff, 0x0000ffffffffffff, 0x0000ffffffffffff,
                0x0000ffffffffffff, 0x0000ffffffffffff, 0x00ffffffffffffff, 0x00ffffffffffffff, 0x00ffffffffffffff, 0x00ffffffffffffff,
                0x00ffffffffffffff, 0x00ffffffffffffff, 0x00ffffffffffffff, 0x00ffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff};
uint64_t    const   mask_latter_ranks[64]       =   {   0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
                0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffff00, 0xffffffffffffff00,
                0xffffffffffffff00, 0xffffffffffffff00, 0xffffffffffffff00, 0xffffffffffffff00, 0xffffffffffffff00, 0xffffffffffffff00,
                0xffffffffffff0000, 0xffffffffffff0000, 0xffffffffffff0000, 0xffffffffffff0000, 0xffffffffffff0000, 0xffffffffffff0000,
                0xffffffffffff0000, 0xffffffffffff0000, 0xffffffffff000000, 0xffffffffff000000, 0xffffffffff000000, 0xffffffffff000000,
                0xffffffffff000000, 0xffffffffff000000, 0xffffffffff000000, 0xffffffffff000000, 0xffffffff00000000, 0xffffffff00000000,
                0xffffffff00000000, 0xffffffff00000000, 0xffffffff00000000, 0xffffffff00000000, 0xffffffff00000000, 0xffffffff00000000,
                0xffffff0000000000, 0xffffff0000000000, 0xffffff0000000000, 0xffffff0000000000, 0xffffff0000000000, 0xffffff0000000000,
                0xffffff0000000000, 0xffffff0000000000, 0xffff000000000000, 0xffff000000000000, 0xffff000000000000, 0xffff000000000000,
                0xffff000000000000, 0xffff000000000000, 0xffff000000000000, 0xffff000000000000, 0xff00000000000000, 0xff00000000000000,
                0xff00000000000000, 0xff00000000000000, 0xff00000000000000, 0xff00000000000000, 0xff00000000000000, 0xff00000000000000};


    /*  Mask file[2]    0 0 0 0 0 1 0 0     Mask file rv[2]     0 0 1 0 0 0 0 0     Not mask file[2]    1 1 1 1 1 0 1 1
                        0 0 0 0 0 1 0 0                         0 0 1 0 0 0 0 0                         1 1 1 1 1 0 1 1
                        0 0 0 0 0 1 0 0                         0 0 1 0 0 0 0 0                         1 1 1 1 1 0 1 1
                        0 0 0 0 0 1 0 0                         0 0 1 0 0 0 0 0                         1 1 1 1 1 0 1 1
                        0 0 0 0 0 1 0 0                         0 0 1 0 0 0 0 0                         1 1 1 1 1 0 1 1
                        0 0 0 0 0 1 0 0                         0 0 1 0 0 0 0 0                         1 1 1 1 1 0 1 1
                        0 0 0 0 0 1 0 0                         0 0 1 0 0 0 0 0                         1 1 1 1 1 0 1 1
                        0 0 0 0 0 1 0 0                         0 0 1 0 0 0 0 0                         1 1 1 1 1 0 1 1 */

uint64_t    const   mask_file[8]                =   {   0x8080808080808080, 0x4040404040404040, 0x2020202020202020, 0x1010101010101010,
                0x0808080808080808, 0x0404040404040404, 0x0202020202020202, 0x0101010101010101};
uint64_t    const   mask_file_rv[8]             =   {   0x0101010101010101, 0x0202020202020202, 0x0404040404040404, 0x0808080808080808,
                0x1010101010101010, 0x2020202020202020, 0x4040404040404040, 0x8080808080808080};
uint64_t    const   not_mask_file[8]            =   {   0x7f7f7f7f7f7f7f7f, 0xbfbfbfbfbfbfbfbf, 0xdfdfdfdfdfdfdfdf, 0xefefefefefefefef,
                0xf7f7f7f7f7f7f7f7, 0xfbfbfbfbfbfbfbfb, 0xfdfdfdfdfdfdfdfd, 0xfefefefefefefefe};



    /*  Knight moves[i]     0 0 0 0 0 0 0 0     King moves[i]   0 0 0 0 0 0 0 0
                            0 0 0 1 0 1 0 0                     0 0 1 1 1 0 0 0
                            0 0 1 0 0 0 1 0                     0 0 1 i 1 0 0 0
                            0 0 0 0 i 0 0 0                     0 0 1 1 1 0 0 0
                            0 0 1 0 0 0 1 0                     0 0 0 0 0 0 0 0
                            0 0 0 1 0 1 0 0                     0 0 0 0 0 0 0 0
                            0 0 0 0 0 0 0 0                     0 0 0 0 0 0 0 0
                            0 0 0 0 0 0 0 0                     0 0 0 0 0 0 0 0  */

uint64_t    const   knight_moves[64]            =   {   0x0000000000020400, 0x0000000000050800, 0x00000000000a1100, 0x0000000000142200,
                0x0000000000284400, 0x0000000000508800, 0x0000000000a01000, 0x0000000000402000, 0x0000000002040004, 0x0000000005080008,
                0x000000000a110011, 0x0000000014220022, 0x0000000028440044, 0x0000000050880088, 0x00000000a0100010, 0x0000000040200020,
                0x0000000204000402, 0x0000000508000805, 0x0000000a1100110a, 0x0000001422002214, 0x0000002844004428, 0x0000005088008850,
                0x000000a0100010a0, 0x0000004020002040, 0x0000020400040200, 0x0000050800080500, 0x00000a1100110a00, 0x0000142200221400,
                0x0000284400442800, 0x0000508800885000, 0x0000a0100010a000, 0x0000402000204000, 0x0002040004020000, 0x0005080008050000,
                0x000a1100110a0000, 0x0014220022140000, 0x0028440044280000, 0x0050880088500000, 0x00a0100010a00000, 0x0040200020400000,
                0x0204000402000000, 0x0508000805000000, 0x0a1100110a000000, 0x1422002214000000, 0x2844004428000000, 0x5088008850000000,
                0xa0100010a0000000, 0x4020002040000000, 0x0400040200000000, 0x0800080500000000, 0x1100110a00000000, 0x2200221400000000,
                0x4400442800000000, 0x8800885000000000, 0x100010a000000000, 0x2000204000000000, 0x0004020000000000, 0x0008050000000000,
                0x00110a0000000000, 0x0022140000000000, 0x0044280000000000, 0x0088500000000000, 0x0010a00000000000, 0x0020400000000000};
uint64_t    const   king_moves[64]              =   {   0x0000000000000302, 0x0000000000000705, 0x0000000000000e0a, 0x0000000000001c14,
                0x0000000000003828, 0x0000000000007050, 0x000000000000e0a0, 0x000000000000c040, 0x0000000000030203, 0x0000000000070507,
                0x00000000000e0a0e, 0x00000000001c141c, 0x0000000000382838, 0x0000000000705070, 0x0000000000e0a0e0, 0x0000000000c040c0,
                0x0000000003020300, 0x0000000007050700, 0x000000000e0a0e00, 0x000000001c141c00, 0x0000000038283800, 0x0000000070507000,
                0x00000000e0a0e000, 0x00000000c040c000, 0x0000000302030000, 0x0000000705070000, 0x0000000e0a0e0000, 0x0000001c141c0000,
                0x0000003828380000, 0x0000007050700000, 0x000000e0a0e00000, 0x000000c040c00000, 0x0000030203000000, 0x0000070507000000,
                0x00000e0a0e000000, 0x00001c141c000000, 0x0000382838000000, 0x0000705070000000, 0x0000e0a0e0000000, 0x0000c040c0000000,
                0x0003020300000000, 0x0007050700000000, 0x000e0a0e00000000, 0x001c141c00000000, 0x0038283800000000, 0x0070507000000000,
                0x00e0a0e000000000, 0x00c040c000000000, 0x0302030000000000, 0x0705070000000000, 0x0e0a0e0000000000, 0x1c141c0000000000,
                0x3828380000000000, 0x7050700000000000, 0xe0a0e00000000000, 0xc040c00000000000, 0x0203000000000000, 0x0507000000000000,
                0x0a0e000000000000, 0x141c000000000000, 0x2838000000000000, 0x5070000000000000, 0xa0e0000000000000, 0x40c0000000000000};



    /*  Bishop masks[i] 0 0 0 0 0 0 0 0         Rook masks[i]   i 1 1 1 1 1 1 0
                        0 0 1 0 1 0 0 0                         1 0 0 0 0 0 0 0
                        0 0 0 i 0 0 0 0                         1 0 0 0 0 0 0 0
                        0 0 1 0 1 0 0 0                         1 0 0 0 0 0 0 0
                        0 1 0 0 0 1 0 0                         1 0 0 0 0 0 0 0
                        0 0 0 0 0 0 1 0                         1 0 0 0 0 0 0 0
                        0 0 0 0 0 0 0 0                         1 0 0 0 0 0 0 0
                        0 0 0 0 0 0 0 0                         0 0 0 0 0 0 0 0  */

uint64_t    const   bishop_masks[64]            =   {   0x0040201008040200, 0x0000402010080400, 0x0000004020100a00, 0x0000000040221400,
                0x0000000002442800, 0x0000000204085000, 0x0000020408102000, 0x0002040810204000, 0x0020100804020000, 0x0040201008040000,
                0x00004020100a0000, 0x0000004022140000, 0x0000000244280000, 0x0000020408500000, 0x0002040810200000, 0x0004081020400000,
                0x0010080402000200, 0x0020100804000400, 0x004020100a000a00, 0x0000402214001400, 0x0000024428002800, 0x0002040850005000,
                0x0004081020002000, 0x0008102040004000, 0x0008040200020400, 0x0010080400040800, 0x0020100a000a1000, 0x0040221400142200,
                0x0002442800284400, 0x0004085000500800, 0x0008102000201000, 0x0010204000402000, 0x0004020002040800, 0x0008040004081000,
                0x00100a000a102000, 0x0022140014224000, 0x0044280028440200, 0x0008500050080400, 0x0010200020100800, 0x0020400040201000,
                0x0002000204081000, 0x0004000408102000, 0x000a000a10204000, 0x0014001422400000, 0x0028002844020000, 0x0050005008040200,
                0x0020002010080400, 0x0040004020100800, 0x0000020408102000, 0x0000040810204000, 0x00000a1020400000, 0x0000142240000000,
                0x0000284402000000, 0x0000500804020000, 0x0000201008040200, 0x0000402010080400, 0x0002040810204000, 0x0004081020400000,
                0x000a102040000000, 0x0014224000000000, 0x0028440200000000, 0x0050080402000000, 0x0020100804020000, 0x0040201008040200};
uint64_t    const   rook_masks[64]              =   {   0x000101010101017e, 0x000202020202027c, 0x000404040404047a, 0x0008080808080876,
                0x001010101010106e, 0x002020202020205e, 0x004040404040403e, 0x008080808080807e, 0x0001010101017e00, 0x0002020202027c00,
                0x0004040404047a00, 0x0008080808087600, 0x0010101010106e00, 0x0020202020205e00, 0x0040404040403e00, 0x0080808080807e00,
                0x00010101017e0100, 0x00020202027c0200, 0x00040404047a0400, 0x0008080808760800, 0x00101010106e1000, 0x00202020205e2000,
                0x00404040403e4000, 0x00808080807e8000, 0x000101017e010100, 0x000202027c020200, 0x000404047a040400, 0x0008080876080800,
                0x001010106e101000, 0x002020205e202000, 0x004040403e404000, 0x008080807e808000, 0x0001017e01010100, 0x0002027c02020200,
                0x0004047a04040400, 0x0008087608080800, 0x0010106e10101000, 0x0020205e20202000, 0x0040403e40404000, 0x0080807e80808000,
                0x00017e0101010100, 0x00027c0202020200, 0x00047a0404040400, 0x0008760808080800, 0x00106e1010101000, 0x00205e2020202000,
                0x00403e4040404000, 0x00807e8080808000, 0x007e010101010100, 0x007c020202020200, 0x007a040404040400, 0x0076080808080800,
                0x006e101010101000, 0x005e202020202000, 0x003e404040404000, 0x007e808080808000, 0x7e01010101010100, 0x7c02020202020200,
                0x7a04040404040400, 0x7608080808080800, 0x6e10101010101000, 0x5e20202020202000, 0x3e40404040404000, 0x7e80808080808000};



uint64_t    const   light_dark_squares[2]       =   {   0x55aa55aa55aa55aa, 0xaa55aa55aa55aa55  };
uint64_t    const   mask_ep_ranks[2]            =   {   0x00000000ff000000, 0x000000ff00000000  };
uint8_t     const   promoting_rank[2]           =   {   0x0001,             0x0006  };
uint8_t     const   adjacent_files[8]           =   {   0x0003,             0x0007,             0x000e,             0x001c,
                0x0038,             0x0070,             0x00e0,             0x00c0  };

uint64_t    const   attack_zones[5][64]         =   {{  0,                  0,                  0,                  0,
                0,                  0,                  0,                  0,                  0,                  0,
                0,                  0,                  0,                  0,                  0,                  0,
                0,                  0,                  0,                  0,                  0,                  0,
                0,                  0,                  0,                  0,                  0,                  0,
                0,                  0,                  0,                  0,                  0,                  0,
                0,                  0,                  0,                  0,                  0,                  0,
                0,                  0,                  0,                  0,                  0,                  0,
                0,                  0,                  0,                  0,                  0,                  0,
                0,                  0,                  0,                  0,                  0,                  0,
                0,                  0,                  0,                  0,                  0,                  0},
                                                     {  0x0000000000030303, 0x0000000000070707, 0x00000000000e0e0e, 0x00000000001c1c1c,     /* Pawn */
                0x0000000000383838, 0x0000000000707070, 0x0000000000e0e0e0, 0x0000000000c0c0c0, 0x0000000003030303, 0x0000000007070707,
                0x000000000e0e0e0e, 0x000000001c1c1c1c, 0x0000000038383838, 0x0000000070707070, 0x00000000e0e0e0e0, 0x00000000c0c0c0c0,
                0x0000000303030303, 0x0000000707070707, 0x0000000e0e0e0e0e, 0x0000001c1c1c1c1c, 0x0000003838383838, 0x0000007070707070,
                0x000000e0e0e0e0e0, 0x000000c0c0c0c0c0, 0x0000030303030300, 0x0000070707070700, 0x00000e0e0e0e0e00, 0x00001c1c1c1c1c00,
                0x0000383838383800, 0x0000707070707000, 0x0000e0e0e0e0e000, 0x0000c0c0c0c0c000, 0x0003030303030000, 0x0007070707070000,
                0x000e0e0e0e0e0000, 0x001c1c1c1c1c0000, 0x0038383838380000, 0x0070707070700000, 0x00e0e0e0e0e00000, 0x00c0c0c0c0c00000,
                0x0303030303000000, 0x0707070707000000, 0x0e0e0e0e0e000000, 0x1c1c1c1c1c000000, 0x3838383838000000, 0x7070707070000000,
                0xe0e0e0e0e0000000, 0xc0c0c0c0c0000000, 0x0303030300000000, 0x0707070700000000, 0x0e0e0e0e00000000, 0x1c1c1c1c00000000,
                0x3838383800000000, 0x7070707000000000, 0xe0e0e0e000000000, 0xc0c0c0c000000000, 0x0303030000000000, 0x0707070000000000,
                0x0e0e0e0000000000, 0x1c1c1c0000000000, 0x3838380000000000, 0x7070700000000000, 0xe0e0e00000000000, 0xc0c0c00000000000},
                                                    {   0x000000050f1f0f1f, 0x0000000a1f3f1f3f, 0x000000153f7f3f7f, 0x0000002a7fff7fff,     /* Knight */
                0x00000054fefffeff, 0x000000a8fcfefcfe, 0x00000050f8fcf8fc, 0x000000a0f0f8f0f8, 0x0000050f1f0f1f0f, 0x00000a1f3f1f3f1f,
                0x0000153f7f3f7f3f, 0x00002a7fff7fff7f, 0x000054fefffefffe, 0x0000a8fcfefcfefc, 0x000050f8fcf8fcf8, 0x0000a0f0f8f0f8f0,
                0x00050f1f0f1f0f1f, 0x000a1f3f1f3f1f3f, 0x00153f7f3f7f3f7f, 0x002a7fff7fff7fff, 0x0054fefffefffeff, 0x00a8fcfefcfefcfe,
                0x0050f8fcf8fcf8fc, 0x00a0f0f8f0f8f0f8, 0x050f1f0f1f0f1f0f, 0x0a1f3f1f3f1f3f1f, 0x153f7f3f7f3f7f3f, 0x2a7fff7fff7fff7f,
                0x54fefffefffefffe, 0xa8fcfefcfefcfefc, 0x50f8fcf8fcf8fcf8, 0xa0f0f8f0f8f0f8f0, 0x0f1f0f1f0f1f0f05, 0x1f3f1f3f1f3f1f0a,
                0x3f7f3f7f3f7f3f15, 0x7fff7fff7fff7f2a, 0xfefffefffefffe54, 0xfcfefcfefcfefca8, 0xf8fcf8fcf8fcf850, 0xf0f8f0f8f0f8f0a0,
                0x1f0f1f0f1f0f0500, 0x3f1f3f1f3f1f0a00, 0x7f3f7f3f7f3f1500, 0xff7fff7fff7f2a00, 0xfffefffefffe5400, 0xfefcfefcfefca800,
                0xfcf8fcf8fcf85000, 0xf8f0f8f0f8f0a000, 0x0f1f0f1f0f050000, 0x1f3f1f3f1f0a0000, 0x3f7f3f7f3f150000, 0x7fff7fff7f2a0000,
                0xfefffefffe540000, 0xfcfefcfefca80000, 0xf8fcf8fcf8500000, 0xf0f8f0f8f0a00000, 0x1f0f1f0f05000000, 0x3f1f3f1f0a000000,
                0x7f3f7f3f15000000, 0xff7fff7f2a000000, 0xfffefffe54000000, 0xfefcfefca8000000, 0xfcf8fcf850000000, 0xf8f0f8f0a0000000},
                                                    {   0xc0e070381c0f0707, 0xc0e0f0783d1f0f0f, 0x80c0e0f17b3f1f1f, 0x0080c1e3f77f3e3e,     /* Bishop */
                0x000183c7effe7c7c, 0x0103078fdefcf8f8, 0x03070f1ebcf8f0f0, 0x03070e1c38f0e0e0, 0xe0f0783c1f0f070f, 0xe0f0f87d3f1f0f1f,
                0xc0e0f1fb7f3f1f3f, 0x80c1e3f7ff7f3e7f, 0x0183c7effffe7cfe, 0x03078fdffefcf8fc, 0x070f1fbefcf8f0f8, 0x070f1e3cf8f0e0f0,
                0xf0783c1f0f070f1f, 0xf0f87d3f1f0f1f3f, 0xe0f1fb7f3f1f3f7f, 0xc1e3f7ff7f3e7fff, 0x83c7effffe7cfeff, 0x078fdffefcf8fcfe,
                0x0f1fbefcf8f0f8fc, 0x0f1e3cf8f0e0f0f8, 0x783c1f0f070f1f3c, 0xf87d3f1f0f1f3f7d, 0xf1fb7f3f1f3f7ffb, 0xe3f7ff7f3e7ffff7,
                0xc7effffe7cfeffef, 0x8fdffefcf8fcfedf, 0x1fbefcf8f0f8fcbe, 0x1e3cf8f0e0f0f83c, 0x3c1f0f070f1f3c78, 0x7d3f1f0f1f3f7df8,
                0xfb7f3f1f3f7ffbf1, 0xf7ff7f3e7ffff7e3, 0xeffffe7cfeffefc7, 0xdffefcf8fcfedf8f, 0xbefcf8f0f8fcbe1f, 0x3cf8f0e0f0f83c1e,
                0x1f0f070f1f3c78f0, 0x3f1f0f1f3f7df8f0, 0x7f3f1f3f7ffbf1e0, 0xff7f3e7ffff7e3c1, 0xfffe7cfeffefc783, 0xfefcf8fcfedf8f07,
                0xfcf8f0f8fcbe1f0f, 0xf8f0e0f0f83c1e0f, 0x0f070f1f3c78f0e0, 0x1f0f1f3f7df8f0e0, 0x3f1f3f7ffbf1e0c0, 0x7f3e7ffff7e3c180,
                0xfe7cfeffefc78301, 0xfcf8fcfedf8f0703, 0xf8f0f8fcbe1f0f07, 0xf0e0f0f83c1e0f07, 0x07070f1c3870e0c0, 0x0f0f1f3d78f0e0c0,
                0x1f1f3f7bf1e0c080, 0x3e3e7ff7e3c18000, 0x7c7cfeefc7830100, 0xf8f8fcde8f070301, 0xf0f0f8bc1e0f0703, 0xe0e0f0381c0e0703},
                                                    {   0x030303030303ffff, 0x070707070707ffff, 0x0e0e0e0e0e0effff, 0x1c1c1c1c1c1cffff,     /* Rook */
                0x383838383838ffff, 0x707070707070ffff, 0xe0e0e0e0e0e0ffff, 0xc0c0c0c0c0c0ffff, 0x0303030303ffffff, 0x0707070707ffffff,
                0x0e0e0e0e0effffff, 0x1c1c1c1c1cffffff, 0x3838383838ffffff, 0x7070707070ffffff, 0xe0e0e0e0e0ffffff, 0xc0c0c0c0c0ffffff,
                0x03030303ffffff03, 0x07070707ffffff07, 0x0e0e0e0effffff0e, 0x1c1c1c1cffffff1c, 0x38383838ffffff38, 0x70707070ffffff70,
                0xe0e0e0e0ffffffe0, 0xc0c0c0c0ffffffc0, 0x030303ffffff0303, 0x070707ffffff0707, 0x0e0e0effffff0e0e, 0x1c1c1cffffff1c1c,
                0x383838ffffff3838, 0x707070ffffff7070, 0xe0e0e0ffffffe0e0, 0xc0c0c0ffffffc0c0, 0x0303ffffff030303, 0x0707ffffff070707,
                0x0e0effffff0e0e0e, 0x1c1cffffff1c1c1c, 0x3838ffffff383838, 0x7070ffffff707070, 0xe0e0ffffffe0e0e0, 0xc0c0ffffffc0c0c0,
                0x03ffffff03030303, 0x07ffffff07070707, 0x0effffff0e0e0e0e, 0x1cffffff1c1c1c1c, 0x38ffffff38383838, 0x70ffffff70707070,
                0xe0ffffffe0e0e0e0, 0xc0ffffffc0c0c0c0, 0xffffff0303030303, 0xffffff0707070707, 0xffffff0e0e0e0e0e, 0xffffff1c1c1c1c1c,
                0xffffff3838383838, 0xffffff7070707070, 0xffffffe0e0e0e0e0, 0xffffffc0c0c0c0c0, 0xffff030303030303, 0xffff070707070707,
                0xffff0e0e0e0e0e0e, 0xffff1c1c1c1c1c1c, 0xffff383838383838, 0xffff707070707070, 0xffffe0e0e0e0e0e0, 0xffffc0c0c0c0c0c0}};

uint64_t            corresponding_files[256];
uint64_t            opposite_masks[64][64];
uint64_t            interceding_squares[64][64];


// =====================================
//  Initializes many of the masks, etc.
// =====================================

void initialize() {
    uint64_t x = 1;
    int i, j, k, l, m;

    x = 1;


    /*  Populate rook and bishop move tables */

    find_rook_magics(12);
    find_bishop_magics(9);

    /*  Pawn checks[1][i]   0 0 0 0 0 0 0 0     Pawn checks[0][i]   0 0 0 0 0 0 0 0
                            0 0 0 0 0 0 0 0                         0 0 0 0 0 0 0 0
                            0 0 0 0 0 0 0 0                         0 1 0 0 0 0 0 0
                            0 0 0 0 i 0 0 0                         i 0 0 0 0 0 0 0
                            0 0 0 1 0 1 0 0                         0 0 0 0 0 0 0 0
                            0 0 0 0 0 0 0 0                         0 0 0 0 0 0 0 0
                            0 0 0 0 0 0 0 0                         0 0 0 0 0 0 0 0
                            0 0 0 0 0 0 0 0                         0 0 0 0 0 0 0 0  */

    for(j = 0; j <= 63; j++){
    x = 0;
    if(j >= 16){
      if((j % 8) > 0) x |= two_to_the[j-9];
      if((j % 8) < 7) x |= two_to_the[j-7];
    }
    pawn_checks[1][j] = x;
    x = 0;
    if(j <= 47){
      if((j % 8) > 0) x |= two_to_the[j+7];
      if((j % 8) < 7) x |= two_to_the[j+9];
    }
    pawn_checks[0][j] = x;
    }

    /* Corresponding files, corresponding_files[00100100]     0 0 1 0 0 1 0 0
                                                              0 0 1 0 0 1 0 0
                                                              0 0 1 0 0 1 0 0
                                                              0 0 1 0 0 1 0 0
                                                              0 0 1 0 0 1 0 0
                                                              0 0 1 0 0 1 0 0
                                                              0 0 1 0 0 1 0 0
                                                              0 0 1 0 0 1 0 0 */

    for(i = 0; i <= 255; i++) {
        corresponding_files[i] = 0;
        for(j = 0; j <= 7; j++)
            if(two_to_the[j] & i)
                corresponding_files[i] |= mask_file_rv[j];
    }




    /*  Opposite masks[i][j]    0 0 0 0 1 0 0 0     Interceding squares[i][j]   0 0 0 0 0 0 0 0 0
                                0 0 0 0 1 0 0 0                                 0 i 0 0 0 0 0 0 0
                                0 0 0 0 1 0 0 0                                 0 0 1 0 0 0 0 0 0
                                0 0 0 0 j 0 0 0                                 0 0 0 1 0 0 0 0 0
                                0 0 0 0 0 0 0 0                                 0 0 0 0 1 0 0 0 0
                                0 0 0 0 i 0 0 0                                 0 0 0 0 0 1 0 0 0
                                0 0 0 0 0 0 0 0                                 0 0 0 0 0 0 j 0 0
                                0 0 0 0 0 0 0 0                                 0 0 0 0 0 0 0 0 0  */

    for(i = 0; i <= 63; i++)
        for(j = 0; j <= 63; j++) {
            opposite_masks[i][j] = 0;
            interceding_squares[i][j] = 0;
        }

    for(i = 0; i <= 63; i++){
        k = i % 8;
        l = (i - k) / 8;
        for(j = k - 1; j >= 0; j--) {       // Right
            for(m = j - 1; m >= 0; m--)
                opposite_masks[i][(8*l)+j] |= two_to_the[(8*l)+m];
            for(m = k - 1; m > j; m--)
                interceding_squares[i][(8*l)+j] |= two_to_the[(8*l)+m];
        }
        for(j = k + 1; j <= 7; j++) {       // Left
            for(m = j + 1; m <= 7; m++)
                opposite_masks[i][(8*l)+j] |= two_to_the[(8*l)+m];
            for(m = k + 1; m < j; m++)
                interceding_squares[i][(8*l)+j] |= two_to_the[(8*l)+m];
        }
        for(j = l - 1; j >= 0; j--) {       // Down
            for(m = j - 1; m >= 0; m--)
                opposite_masks[i][(8*j)+k] |= two_to_the[(8*m)+k];
            for(m = l - 1; m > j; m--)
                interceding_squares[i][(8*j)+k] |= two_to_the[(8*m)+k];
        }
        for(j = l + 1; j <= 7; j++) {       // Up
            for(m = j + 1; m <= 7; m++)
                opposite_masks[i][(8*j)+k] |= two_to_the[(8*m)+k];
            for(m = l + 1; m < j; m++)
                interceding_squares[i][(8*j)+k] |= two_to_the[(8*m)+k];
        }

        for(j = 1; ((k + j) <= 7) && ((l + j) <= 7); j++) {     // Up, left
            for(m = j+1; ((k + m) <= 7) && ((l + m) <= 7); m++)
                opposite_masks[i][(8*(l+j))+(k+j)] |= two_to_the[(8*(l+m))+(k+m)];
            for(m = 1; m < j; m++)
                interceding_squares[i][(8*(l+j))+(k+j)] |= two_to_the[(8*(l+m))+(k+m)];
        }
        for(j = 1; ((k + j) <= 7) && ((l - j) >= 0); j++) {     // Down, left
            for(m = j+1; ((k + m) <= 7) && ((l - m) >= 0); m++)
                opposite_masks[i][(8*(l-j))+(k+j)] |= two_to_the[(8*(l-m))+(k+m)];
            for(m = 1; m < j; m++)
                interceding_squares[i][(8*(l-j))+(k+j)] |= two_to_the[(8*(l-m))+(k+m)];
        }
        for(j = 1; ((k - j) >= 0) && ((l + j) <= 7); j++) {     // Up, right
            for(m = j+1; ((k - m) >= 0) && ((l + m) <= 7); m++)
                opposite_masks[i][(8*(l+j))+(k-j)] |= two_to_the[(8*(l+m))+(k-m)];
            for(m = 1; m < j; m++)
                interceding_squares[i][(8*(l+j))+(k-j)] |= two_to_the[(8*(l+m))+(k-m)];
        }
        for(j = 1; ((k - j) >= 0) && ((l - j) >= 0); j++) {     // Down right
            for(m = j+1; ((k - m) >= 0) && ((l - m) >= 0); m++)
                opposite_masks[i][(8*(l-j))+(k-j)] |= two_to_the[(8*(l-m))+(k-m)];
            for(m = 1; m < j; m++)
                interceding_squares[i][(8*(l-j))+(k-j)] |= two_to_the[(8*(l-m))+(k-m)];
        }
    }


}