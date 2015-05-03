#include <iostream>
#include <stdint.h>
#include <string>
#include "bb.cpp"
#include "cboard.cpp"

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
    CBoard board;
    board.move(E2, E4);
    board.print();
    return 0;
}
