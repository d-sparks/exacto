#include <iostream>
#include <stdint.h>
#include <string.h>
#include "bb.cpp"
#include "cgame.cpp"
#include "moves.cpp"
#include "masks.cpp"

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
    CGame game;
    magics::populateRookTables();
    magics::populateBishopTables();
    masks::generateOpposite();
    masks::generateInterceding();

    string userInput;

    for(cin >> userInput; true; cin >> userInput) {
        if(userInput == "usermove") {
            mv candidateMoves[256];
            game.moveGen(candidateMoves);
            mv move = moves::cinMove(candidateMoves);
            game.makeMove(&move);
        }

        if(userInput == "print") {
            game.print();
        }

        if(userInput == "divide") {
            int depth;
            cin >> depth;
            divide(&game, depth);
        }

        if(userInput == "setboard") {
            string POS, WTM, CAS, EPT, HMC, FMC;
            cin >> POS;
            cin >> WTM;
            cin >> CAS;
            cin >> EPT;
            // cin >> HMC;
            // cin >> FMC;
            game.setBoard(POS, WTM, CAS, EPT);
        }

        if(userInput == "quit" || userInput == "exit") {
            break;
        }
    }

    return 0;
}
