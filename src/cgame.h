#pragma once
#include <string.h>
#include "move.h"
#include "cboard.h"

using namespace std;

class CGame : public CBoard {
public:
    void makeMove(mv m);
    void unmakeMove(mv m);
    CGame();
    CGame(string brd, string clr, string cstl, string ep, string hm, string fm);
    ~CGame();
};
