#include <stdint.h>
#include <iostream>
#include <string>

#include "bitboard.h"
#include "exacto.h"
#include "defines.h"
#include "game.h"
#include "magics.h"
#include "masks.h"
#include "moves.h"

using namespace exacto;

namespace {

// greet is the classic exacto greeting, dating back to 0.a.
void greet(const std::string& version) {
  std::cout << std::endl
            << "   exacto[" << version << "]          " << std::endl
            << " <<------------------------------->>  " << std::endl
            << "             by Daniel Sparks, USA    " << std::endl
            << std::endl
            << "Type 'game' for gameplay commands, or 'help' for all commands."
            << std::endl << std::endl;
}

}  // namespace

int main() {
  greet("?.?");
  Game game;
  Exacto exacto(game);
  magics::populateRookTables();
  magics::populateBishopTables();
  masks::init();

  std::cout << exacto.Search(&game, -10000, 10000, 1, 0) << std::endl;

  std::string userInput;

  for (std::cin >> userInput; true; std::cin >> userInput) {
    if (userInput == "usermove") {
      Move candidateMoves[256];
      game.MoveGen(candidateMoves);
      Move move;
      moves::ReadMoveFromStdin(candidateMoves, &move);
      game.makeMove(&move);
    }

    if (userInput == "go") {
      exacto.Go(&game);
    }

    if (userInput == "print") {
      game.print();
    }

    if (userInput == "divide") {
      int depth;
      std::cin >> depth;
      divide(&game, depth);
    }

    if (userInput == "perft") {
      int depth;
      std::cin >> depth;
      std::cout << perft(&game, depth) - perft(&game, depth - 1) << std::endl;
    }

    if (userInput == "clear") {
      exacto.hash.clear_table();
    }

    if (userInput == "setboard") {
      std::string POS, WTM, CAS, EPT, HMC, FMC;
      std::cin >> POS;
      std::cin >> WTM;
      std::cin >> CAS;
      std::cin >> EPT;
      // std::cin >> HMC;
      // std::cin >> FMC;
      game.setBoard(POS, WTM, CAS, EPT);
    }

    if (userInput == "quit" || userInput == "exit") {
      break;
    }
  }

  return 0;
}
