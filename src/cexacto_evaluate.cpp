#include "cexacto_evaluate.h"
#include "cexacto.h"
#include "cgame.h"

int16_t CExacto::evaluate(CGame* game) {
    int16_t score = 0;
    int16_t PVT[2][7][64] = {
        // BLACK
        {
            //  NONE
            { 0 },
            // BLACK:PAWN
            {
                0, 0, 0, 0, 0, 0, 0, 0,
                -140, -145, -150, -150, -150, -145, -140, -140,
                -130, -130, -140, -140, -140, -130, -130, -130,
                -110, -110, -120, -130, -130, -125, -120, -110,
                -85, -85, -100, -115, -115, -110, -95, -90,
                -95, -100, -85, -105, -105, -100, -100, -95,
                -100, -100, -100, -90, -90, -95, -100, -100,
                0, 0, 0, 0, 0, 0, 0, 0
            },
            // BLACK:KNIGHT
            {
                -250, -260, -280, -300, -300, -280, -260, -250,
                -260, -270, -290, -310, -310, -290, -270, -260,
                -280, -300, -310, -315, -315, -310, -300, -280,
                -300, -310, -320, -325, -325, -320, -310, -300,
                -300, -310, -320, -325, -325, -320, -310, -300,
                -280, -300, -310, -315, -315, -310, -300, -280,
                -260, -270, -290, -310, -310, -290, -270, -260,
                -250, -260, -280, -300, -300, -280, -260, -250
            },
            // BLACK:BISHOP
            {
                -300, -290, -280, -300, -300, -280, -290, -300,
                -300, -310, -300, -310, -310, -300, -310, -300,
                -305, -315, -320, -315, -315, -320, -315, -305,
                -310, -315, -320, -325, -325, -320, -315, -310,
                -310, -315, -320, -325, -325, -320, -315, -310,
                -305, -315, -320, -315, -315, -320, -315, -305,
                -300, -310, -300, -310, -310, -300, -310, -300,
                -300, -290, -280, -300, -300, -280, -290, -300
            },
            // BLACK:ROOK
            { -ROOK_VAL },
            // BLACK:QUEEN
            { -QUEEN_VAL },
            // BLACK:KING
            {
                -15, -15, -20, -25, -25, -20, -15, -15,
                -10, -15, -15, -20, -20, -15, -15, -10,
                -5, -10, -10, -15, -15, -10, -10, -5,
                0, 0, 0, 0, 0, 0, 0, 0,
                30, 35, 40, 50, 50, 40, 35, 30,
                20, 25, 30, 35, 35, 30, 25, 20,
                10, 15, 20, 25, 25, 20, 15, 10,
                -10, -20, -5, 5, 15, -20, -25, -10
            }
        },
        // WHITE
        {
            //  NONE
            { 0 },
            // WHITE:PAWN
            {
                0, 0, 0, 0, 0, 0, 0, 0,
                100, 100, 100, 90, 90, 95, 100, 100,
                95, 100, 85, 105, 105, 100, 100, 95,
                85, 85, 100, 115, 115, 110, 95, 90,
                110, 110, 120, 130, 130, 125, 120, 110,
                130, 130, 140, 140, 140, 130, 130, 130,
                140, 145, 150, 150, 150, 145, 140, 140,
                0, 0, 0, 0, 0, 0, 0, 0,
            },
            // WHITE:KNIGHT
            {
                250, 260, 280, 300, 300, 280, 260, 250,
                260, 270, 290, 310, 310, 290, 270, 260,
                280, 300, 310, 315, 315, 310, 300, 280,
                300, 310, 320, 325, 325, 320, 310, 300,
                300, 310, 320, 325, 325, 320, 310, 300,
                280, 300, 310, 315, 315, 310, 300, 280,
                260, 270, 290, 310, 310, 290, 270, 260,
                250, 260, 280, 300, 300, 280, 260, 250
            },
            // WHITE:BISHOP
            {
                300, 290, 280, 300, 300, 280, 290, 300,
                300, 310, 300, 310, 310, 300, 310, 300,
                305, 315, 320, 315, 315, 320, 315, 305,
                310, 315, 320, 325, 325, 320, 315, 310,
                310, 315, 320, 325, 325, 320, 315, 310,
                305, 315, 320, 315, 315, 320, 315, 305,
                300, 310, 300, 310, 310, 300, 310, 300,
                300, 290, 280, 300, 300, 280, 290, 300
            },
            // WHITE:ROOK
            { ROOK_VAL },
            // WHITE:QUEEN
            { QUEEN_VAL },
            // WHITE:KING
            {
                10, 20, 5, -5, -15, 20, 25, 10,
                -10, -15, -20, -25, -25, -20, -15, -10,
                -20, -25, -30, -35, -35, -30, -25, -20,
                -30, -35, -40, -50, -50, -40, -35, -30,
                0, 0, 0, 0, 0, 0, 0, 0,
                5, 10, 10, 15, 15, 10, 10, 5,
                10, 15, 15, 20, 20, 15, 15, 10,
                15, 15, 20, 25, 25, 20, 15, 15
            }
        }
    };

    for(ind color = BLACK; color <= WHITE; color++) {
        for(ind piece = PAWN; piece <= KING; piece++) {
            if(piece == ROOK || piece == QUEEN) {
                continue;
            }
            BB pieceboard = game->pieces[color][piece];
            for(ind sq = bitscan(pieceboard); pieceboard; sq = bitscan(pieceboard)) {
                score += PVT[color][piece][sq];
                pieceboard ^= exp_2(sq);
            }
        }
    }

    score += popcount(game->pieces[WHITE][ROOK] * ROOK_VAL);
    score += popcount(game->pieces[WHITE][QUEEN] * QUEEN_VAL);
    score -= popcount(game->pieces[BLACK][ROOK] * ROOK_VAL);
    score -= popcount(game->pieces[BLACK][QUEEN] * QUEEN_VAL);

    return game->wtm? score : -score;
}
