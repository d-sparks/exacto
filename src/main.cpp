#include <iostream>
#include <stdint.h>
#include <string.h>
#include "bb.cpp"
#include "cgame.cpp"
#include "moves.cpp"

using namespace std;

// printVersion is the classis exacto greeting, dating back to 0.a.
void printVersion(string version) {
    cout << endl;
    cout << "   exacto[" << version << "]          " << endl;
    cout << " <<------------------------------->>  " << endl;
    cout << "             by Daniel Sparks, USA    " << endl;
    cout << endl;
    cout << "Type 'game' for gameplay commands, or 'help' for all commands." << endl;
    cout << endl;
}

int main() {
    printVersion("?.?");
    CGame game1, game2;
    game1.print();

    mv move = moves::make(E2, E4, PAWN, NONE, NONE, NONE, REGULAR_MOVE);
    game1.makeMove(move);
    game1.unmakeMove(move);
    cout << (game1 == game2) << endl;
    return 0;
}
