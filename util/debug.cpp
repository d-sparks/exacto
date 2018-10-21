#define _DEBUG 1

#include <stdint.h>
#include <iostream>
#include <string>
#include "../src/bb.cpp"
#include "../src/cexacto.cpp"
#include "../src/cgame.cpp"
#include "../src/masks.cpp"
#include "../src/moves.cpp"

using namespace std;

int main() {
  CGame game;
  Exacto exacto(game);
  magics::populateRookTables();
  magics::populateBishopTables();
  masks::init();

  while (true) {
    // TODO: make a sequence of random moves     
    while (true) {
      exacto.go(&game);
    }
    break;
  }

  return 0;
}

