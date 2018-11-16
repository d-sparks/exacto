#define _DEBUG 1

#include <iostream>
#include <string>

#include "../src/exacto.cpp"
#include "../src/game.cpp"

using namespace std;
using namespace exacto;

int main() {
  Game game;
  Exacto exacto;

  while (true) {
    // TODO: make a sequence of random moves
    while (true) {
      exacto.Go(&game);
    }
    break;
  }

  return 0;
}

