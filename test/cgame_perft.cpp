#include "assert.h"
#include "../src/cgame.cpp"
#include "../src/moves.cpp"
#include "../src/magics.cpp"

using namespace std;

int test1() {
    cout << "Running perft test 1..." << endl;

    CGame game;
    mv move = moves::make(G2, G3, 0, 0, 0, 0, 0);
    cout << "Nodes: " << divide(&game, 2) << endl;

    return 1;
}

int main() {
    int t = 0;

    t += test1();

    cout << endl;
    cout << t << " test(s) OK" << endl;
    return 0;
}
