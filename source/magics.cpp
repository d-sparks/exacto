// magics.cpp
// created: 12:51 AM 12/13/13
// author: Daniel Sparks
// license: See text/license.txt
// purpose: Generation of magic bitboards / population of move tables.

#include "exacto.h"
#include "inlines.h"

/*  These bishop magics are 9 bit magics, and the rook magics are 12 bit.   */

uint64_t    const   bishop_magics[64]           =   {   0x150510001100a407, 0x1020040820240400, 0x4801400808800208, 0x20469c0108004001,
                0x2390084002941,    0x11a120201808041c, 0x185000b00900080,  0x801801080984001,  0x40022a84420201,   0x1000120010304202,
                0x1140004a52008080, 0x860042030281010,  0x405c10000001,     0x300c03022b10214,  0x1212941ac002000,  0x6031210a00a4084,
                0x8000500282300410, 0x8009002800602044, 0x2004000085000600, 0x2060a8f0050004,   0x4410482200500003, 0x82090020d000,
                0xa404006a0500100,  0x2011022040048020, 0x484012041002680,  0x100808020040100,  0xc0050040388080c0, 0x20280201004008,
                0x1001041024008,    0x4058820036220020, 0x600a011022003000, 0x1380380800c12040, 0xa002824040a10420, 0xa02c305108880052,
                0x61000080042,      0x3200020080080080, 0xc2004040000c030,  0x14010020c0000920, 0x20840301011082,   0xa400682204252,
                0x500008010a942080, 0x8004086880320100, 0x34110a6010a00200, 0x86400e20080,      0x808400403000818,  0x1330d02010100020,
                0x21002490030001a,  0x9c1800802a102,    0xcd02280040a10040, 0x2488420820100000, 0x2e11118022010041, 0x1400400a000,
                0x838010440308019,  0x80014304002124,   0x2204848008002,    0x2008452801010100, 0x2040400482200120, 0xa000a208a98,
                0x860244212005060,  0x8400200058410080, 0x10011201002824,   0x4100041080108,    0x4050210a29080040, 0x508204090680};

uint64_t    const   rook_magics[64]             =   {   0xa8002c000108020,  0x100084000802070,  0x8180048020009000, 0x102002110000800,
                0x100023302040800,  0x220012000884b108, 0x2010158412000040, 0x880004024800100,  0x1a41800010400020, 0xba00500c800,
                0x2000c8120005,     0x200080a002840,    0x2e00600800900c,   0x408004808022204,  0x8002440a24040082, 0x19400244008020,
                0x1003428000108024, 0x81020108003c0100, 0x42800820100100,   0x6c00204200030040, 0x4082000811a40100, 0x4820848820008040,
                0x920100a00004008,  0x2081820000241041, 0x9009214104008000, 0x214016004090,     0x804108040080041,  0x1204024010010440,
                0x828040080020800,  0xd02040028040486,  0x18004c020800100,  0x8000040860044080, 0x8030021008600021, 0x80488100404,
                0x40420110880008,   0x1401401023020580, 0x2041000605000410, 0x3c08400c041000b3, 0x8400120090c02485, 0x10200804002a0,
                0x6100a21402000,    0xa2108008085008,   0x1826080004001000, 0x120040a42002000a, 0xc004128000888,    0x80041004c0,
                0x8140184000442c80, 0x4000010298420004, 0x100403000a51008,  0x8888040900c6020,  0x68000a00200420,   0xb0300a0084400200,
                0x200c02400020040,  0x8030004220508,    0xa04a0001000480,   0x20020008810,      0x11b0080002241,    0xd004284000104901,
                0xd808c200104282,   0x420201a90000805,  0x100900500480001,  0x40004980a440011,  0x110048420520401,  0x800011040280c422};


uint64_t quickhash(uint64_t bb, uint64_t key, int bits) {
    return (((bb*key) >> (64 - bits)));
}

// ===================================================================================
//  Random 64 bitstrings with low number of set bits, code credited to: Tord Romstad.
// ===================================================================================

uint64_t random_uint64() {
  uint64_t u1, u2, u3, u4;
  u1 = (uint64_t)(rand()) & 0xFFFF; u2 = (uint64_t)(rand()) & 0xFFFF;
  u3 = (uint64_t)(rand()) & 0xFFFF; u4 = (uint64_t)(rand()) & 0xFFFF;
  return u1 | (u2 << 16) | (u3 << 32) | (u4 << 48);
}

uint64_t random_uint64_fewbits() {
  return random_uint64() & random_uint64() & random_uint64();
}

uint64_t find_rook_magic(int square, int bits) {

    // This function populates rook moves table; to have it operate as a find magics file, swap commented portions

    uint64_t magic, x, y, right_occ, left_occ, up_occ, down_occ;
    uint64_t right = 0, left = 0, i, k, l, m, n;
    int o, up = 0, down = 0;
    bool incomplete = true;

    if((square % 8) < 6) left = 6 - (square % 8);
    if((square % 8) > 1) right = (square % 8) - 1;
    if(((square - (square % 8)) / 8) < 6) up = 6 - ((square - (square % 8)) / 8);
    if(((square - (square % 8)) / 8) > 1) down = ((square - (square % 8)) / 8) - 1;

    while(incomplete) {
        incomplete = false;

        for(i = 0; i < 4096; i++) rook_moves[square][i] = all_set; //(uncomment this line for magic generation)

        //magic = random_uint64_fewbits(); //(uncomment this line for magic generation)

        magic = rook_magics[square];  //(comment this line out for magic generation)

        for(k = 0; k <= (two_to_the[right]-1); k++){
            for(l = 0; l <= (two_to_the[left]-1); l++) {
                for(m = 0; m <= (two_to_the[up]-1); m++) {
                    for(n = 0; n <= (two_to_the[down]-1); n++ ) {

                        // Generate all possible occupancy bitboards for the appropriate mask
                        right_occ = k << ((square - (square % 8)) + 1);
                        left_occ = l << ((square - (square % 8)) + ((square % 8)+1));
                        up_occ = 0;
                        down_occ = 0;
                        for(o = 0; o <= up; o++) up_occ += ((m & two_to_the[o]) << (square + ((7*o) + 8)));
                        for(o = 0; o <= down; o++) down_occ += ((n & two_to_the[o]) << (square - ((9*o) + 8)));
                        x = up_occ + down_occ + right_occ + left_occ;

                        // Generate the corresponding attacks bitboard to hash
                        y = 0;
                        for(o = square; ((o % 8) < 7) && ((two_to_the[o] & x) == 0);){o++; y += two_to_the[o];}
                        for(o = square; (((o - (o%8))/8) < 7) && ((two_to_the[o] & x) == 0);){o += 8; y += two_to_the[o];}
                        for(o = square; ((o % 8) > 0) && ((two_to_the[o] & x) == 0);){o--; y += two_to_the[o];}
                        for(o = square; (((o - (o%8))/8) > 0) && ((two_to_the[o] & x) == 0);){o -= 8; y += two_to_the[o];}

                        if(rook_moves[square][quickhash(x, magic, bits)] == all_set){
                            // A new hash entry
                            rook_moves[square][quickhash(x, magic, bits)] = y;
                        } else if(rook_moves[square][quickhash(x, magic, bits)] != y) {
                            // A bad collision
                            //incomplete = true; //(uncomment this line for magic generation)
                            cout << "Bummer, dude.  The rook_magic for square " << square << " is not magical after all.\n"; //(comment out for generation)
                            //k = two_to_the[right];
                            //l = two_to_the[left];
                            //m = two_to_the[up];
                            //n = two_to_the[down];
                        }

                    }}}}
    }
    return magic;
    return 0;
}

uint64_t find_bishop_magic(int square, int bits) {

    // This function populates bishop moves table; to have it operate as a find magics file, swap commented portions

    uint64_t magic, x, y, nw_occ, sw_occ, se_occ, ne_occ;
    int o, nw = 0, sw = 0, se = 0, ne = 0, i, j, k, l, m, n;;
    bool incomplete = true;

    //uint64_t table[65536];  // max bits = 16

    i = square % 8;
    j = (square - i) / 8;

    if((i > 1) && (j < 6)) nw = (i - 1 <= 6 - j) ? i - 1 : 6 - j; //min(i - 1, 6 - j);
    if((i > 1) && (j > 1)) sw = (i - 1 <= j - 1) ? i - 1 : j - 1; //min(i - 1, j - 1);
    if((i < 6) && (j > 1)) se = (6 - i <= j - 1) ? 6 - i : j - 1; //min(6 - i, j - 1);
    if((i < 6) && (j < 6)) ne = (6 - i <= 6 - j) ? 6 - i : 6 - j; //min(6 - i, 6 - j);

    while(incomplete) {
        incomplete = false;

        for(i = 0; i < 512; i++) bishop_moves[square][i] = all_set; //(uncomment this line for magic generation)

        //magic = random_uint64_fewbits(); //(uncomment this line for magic generation)

        magic = bishop_magics[square];  //(comment this line out for magic generation)

        for(k = 0; k <= (two_to_the[nw]-1); k++){
            for(l = 0; l <= (two_to_the[sw]-1); l++) {
                for(m = 0; m <= (two_to_the[se]-1); m++) {
                    for(n = 0; n <= (two_to_the[ne]-1); n++ ) {
                        nw_occ = 0;
                        sw_occ = 0;
                        se_occ = 0;
                        ne_occ = 0;

                        // Generate all possible occupancy bitboards for the appropriate mask
                        for(o = 0; o <= nw; o++) nw_occ += ((k & two_to_the[o]) << (square + ((6*o) + 7)));
                        for(o = 0; o <= sw; o++) sw_occ += ((l & two_to_the[o]) << (square - ((10*o) + 9)));
                        for(o = 0; o <= se; o++) se_occ += ((m & two_to_the[o]) << (square - ((8*o) + 7)));
                        for(o = 0; o <= ne; o++) ne_occ += ((n & two_to_the[o]) << (square + ((8*o) + 9)));
                        x = nw_occ + sw_occ + se_occ + ne_occ;

                        // Generate the corresponding attacks bitboard to hash
                        y = 0;

                        for(o = square, i = o%8, j = (o-i)/8; (i > 0) && (j < 7) && ((two_to_the[o] & x) == 0); i = o % 8, j = (o-i)/8){o += 7; y += two_to_the[o];}
                        for(o = square, i = o%8, j = (o-i)/8; (i > 0) && (j > 0) && ((two_to_the[o] & x) == 0); i = o % 8, j = (o-i)/8){o -= 9; y += two_to_the[o];}
                        for(o = square, i = o%8, j = (o-i)/8; (i < 7) && (j > 0) && ((two_to_the[o] & x) == 0); i = o % 8, j = (o-i)/8){o -= 7; y += two_to_the[o];}
                        for(o = square, i = o%8, j = (o-i)/8; (i < 7) && (j < 7) && ((two_to_the[o] & x) == 0); i = o % 8, j = (o-i)/8){o += 9; y += two_to_the[o];}

                        if(bishop_moves[square][quickhash(x, magic, bits)] == all_set){
                            // A new hash entry
                            bishop_moves[square][quickhash(x, magic, bits)] = y;
                        } else if(bishop_moves[square][quickhash(x, magic, bits)] != y) {
                            // A bad collision
                            //incomplete = true; //(uncomment this line for magic generation)
                            cout << "Bummer, dude.  The rook_magic for square " << square << " is not magical after all.\n"; //(comment out for generation)
                            //k = two_to_the[nw];
                            //l = two_to_the[sw];
                            //m = two_to_the[se];
                            //n = two_to_the[ne];
                        }

                    }}}}
    }
    return magic;
    return 0;
}

void find_rook_magics(int bits) {
    /*uint64_t x;
    ofstream text;
    text.open("logs/magics_rook.txt");
    text << "uint64_t\tconst\trook_magics[64]\t\t\t\t=\t{\t";*/
    for(int i = 0; i <= 63; i++){
        /*text << "0x" << hex <<*/ find_rook_magic(i, bits);
        /*if(i < 63) {
            text << ",";
            if((i % 6) == 3) text << endl << "\t\t\t\t";
            else text << "\t";
        }*/
    }
    /*text << "};";
    text.close();*/
}



void find_bishop_magics(int bits) {
    /*uint64_t x;
    ofstream text;
    text.open("logs/magics_bishop.txt");
    text << "uint64_t\tconst\tbishop_magics[64]\t\t\t=\t{\t";*/
    for(int i = 0; i <= 63; i++){
        /*text << "0x" << hex <<*/ find_bishop_magic(i, bits);
        /*if(i < 63) {
            text << ",";
            if((i % 6) == 3) text << endl << "\t\t\t\t";
            else text << "\t";
        }*/
    }
    /*text << "};";
    text.close();*/
}
