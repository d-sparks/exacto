#include "assert.h"
#include "../src/cgame.cpp"
#include "../src/moves.cpp"
#include "../src/magics.cpp"

using namespace std;

// Browses the game tree and counts the number of nodes
BB perft(CGame * game, int depth) {
    BB nodes = 1;

    if(depth > 0) {
        mv moves[256];
        game->moveGen(moves);
        for(mv *move = moves; *move; move++) {
            game->makeMove(move);
            nodes += perft(game, depth - 1);
            game->unmakeMove(*move);
        }
    }

    return nodes;
}

// Browse game tree and count nodes, list descendent counts for each child
BB divide(CGame * game, int depth) {
    BB nodes = 0;
    BB descendents = 0;

    mv moves[256];
    game->moveGen(moves);
    for(mv *move = moves; *move; move++) {
        game->makeMove(move);
        cout << moves::algebraic(*move) << " ";
        if(depth >= 2) {
            descendents = perft(game, depth - 1) - perft(game, depth - 2);
        } else {
            descendents = 1;
        }
        game->unmakeMove(*move);
        cout << descendents << endl;
        nodes += descendents;
    }

    return nodes;
}

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
