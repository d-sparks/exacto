#pragma once
#include <string.h>
#include "moves.h"
#include "cboard.h"

using namespace std;

class CGame : public CBoard {
friend class CExacto;
public:
    void makeMove(mv * m);
    void unmakeMove(mv m);
    bool operator==(const CGame &other) const;
    CGame(
        string brd="rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR",
        string clr="w",
        string cstl="KQkq",
        string ep="-",
        string hm="0",
        string fm="0"
    );
    ~CGame();
private:
    int halfMoves;
    int moveNumber;
};
