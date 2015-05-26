#pragma once
#include <iostream>
#include <string.h>
#include <vector>
#include "bb.h"
#include "inlines.h"
#include "moves.h"
#include "squares.cpp"
#include "cgame.h"

using namespace std;

namespace moves {
    ind source(mv m) {
        return m & 63;
    }

    ind dest(mv m) {
        return (m >> 6) & 63;
    }

    ind attacker(mv m) {
        return (m >> 12) & 7;
    }

    ind defender(mv m) {
        return (m >> 15) & 7;
    }

    ind enPassant(mv m) {
        return (m >> 18) & 15;
    }

    ind castling(mv m) {
        return (m >> 22) & 15;
    }

    ind special(mv m) {
        return (m >> 26);
    }

    mv make(mv source, mv dest, mv attacker, mv defender, mv enPassant, mv castling, mv special) {
        return source | (dest << 6)
                      | (attacker << 12)
                      | (defender << 15)
                      | (enPassant << 18)
                      | (castling << 22)
                      | (special << 26);
    }

    string algebraic(mv m) {
        return squares::algebraic[source(m)] + squares::algebraic[dest(m)];
    }

    // Castling is encoded into four bits in a move in the 22-25 indices, representing KQkq
    // respectively.
    mv castlingEncode(BB castlingData) {
        mv out = 0;
        out |= (exp_2(G1) & castlingData) << (22 - G1);
        out |= (exp_2(C1) & castlingData) << (23 - C1);
        out |= (exp_2(G8) & castlingData) >> (G8 - 24);
        out |= (exp_2(C8) & castlingData) >> (C8 - 25);
        return out;
    }

    // Decodes a castlingCode as outputted by castlingEncode.
    BB castlingDecode(ind castlingCode) {
        BB out = 0;
        out |= (BB)(1 & castlingCode) << (G1 - 0);
        out |= (BB)(2 & castlingCode) << (C1 - 1);
        out |= (BB)(4 & castlingCode) << (G8 - 2);
        out |= (BB)(8 & castlingCode) << (C8 - 3);
        return out;
    }

    // Print a move
    void print(mv move) {
        cout << algebraic(move);
    }

    // Create a mv from cin input
    mv cinMove(mv *candidateMoves) {
        string moveString;
        cin >> moveString;
        string sourceString = moveString.substr(0,2);
        string destString = moveString.substr(2,2);
        ind sourceInd = squares::index(sourceString);
        ind destInd = squares::index(destString);

        vector<mv> matches;
        mv out;

        for(mv *move = candidateMoves; *move; move++) {
            if(sourceInd == source(*move) && destInd == dest(*move)) {
                matches.push_back(*move);
            }
        }

        if(matches.size() == 1) {
            out = matches.at(0);
        } else {
            // determine pawn promotion type
        }
        return out;
    }

}
