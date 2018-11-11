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

static const char kVersion[] = "?.?";

// greet is the classic exacto greeting, dating back to 0.a.
void greet() {
  std::cout << std::endl
            << "   exacto[" << kVersion << "]          " << std::endl
            << " <<------------------------------->>  " << std::endl
            << "             by Daniel Sparks, USA    " << std::endl
            << std::endl
            << "Type 'game' for gameplay commands, or 'help' for all commands."
            << std::endl << std::endl;
}

bool REPL(Game& game, Exacto& exacto, std::string input) {
  // Exacto specific commands
  if (input == "greet") {
    greet();
  } else if (input == "print") {
    game.print();
  } else if (input == "divide") {
    int depth;
    std::cin >> depth;
    divide(&game, depth);
  } else if (input == "perft") {
    int depth;
    std::cin >> depth;
    std::cout << perft(&game, depth) - perft(&game, depth - 1) << std::endl;
  } else if (input == "clear") {
    exacto.hash.clear_table();
  }
  // TODO: FEN, move history, stats, help

  // XBoard protocol
  if (input == "quit" || input == "exit") {
    return false;
  } else if (input == "xboard" || input == "winboard") {
    exacto.post_pretty = false;
  } else if (input == "protover") {
    std::cin >> input;  // TODO:
    std::cout << "feature ping=1 setboard=1 playother=1 san=0 usermove=1 "
              << "time=1 draw=0 sigint=0 sigterm=0 reuse=1 analyze=0 "
              << "colors=0 ics=0 name=0 pause=0 done=1" << std::endl;
    return true;
  } else if (input == "accepted") {
    std::cin >> input;
    return true;
  } else if (input == "ping") {
    std::cin >> input;
    std::cout << "pong " << input << std::endl;
    return true;
  } else if (input == "force") {
    exacto.force = !exacto.force;
  } else if (input == "draw") {
    // TODO:
  } else if (input == "usermove") {
    Move legal_moves[256];
    game.MoveGen(legal_moves);
    Move move;
    moves::ReadMoveFromStdin(legal_moves, &move);
    game.MakeMove(&move);
    if (!exacto.force) {
      exacto.Go(&game);
    }
  } else if (input == "go") {
    exacto.Go(&game);
  } else if (input == "undo") {
    // TODO: store move history in Game
  } else if (input == "new") {
    game.SetGame();
  } else if (input == "setboard") {
    std::string POS, WTM, CAS, EPT, HMC, FMC;
    std::cin >> POS;
    std::cin >> WTM;
    std::cin >> CAS;
    std::cin >> EPT;
    // std::cin >> HMC;
    // std::cin >> FMC;
    game.SetBoard(POS, WTM, CAS, EPT);
  } else if (input == "post") {
    exacto.post = true;
    std::cout << "Post ON" << std::endl;
  } else if (input == "nopost") {
    exacto.post = false;
    std::cout << "Post OFF" << std::endl;
  } else if (input == "version") {
    std::cout << kVersion << std::endl;
  } else if (input == "hash") {
    int size_in_megabytes;
    std::cin >> size_in_megabytes;
    exacto.hash.set_dimension(size_in_megabytes);
  } else if (input == "name") {
  }

  // Time control specific
  if (input == "level") {
    int MPS, base, increment;
    std::cin >> MPS;
    std::cin >> base;
    std::cin >> increment;
    exacto.SetLevels(MPS, base * 60 * 100, increment * 100);
  } else if (input == "st") {
  } else if (input == "time") {
    int centiseconds;
    std::cin >> centiseconds;
    exacto.set_time(centiseconds);
  } else if (input == "otim") {
  } else if (input == "mt") {
  }

  return true;
}

}  // namespace

int main() {
  Game game;
  Exacto exacto(game);
  magics::PopulateRookTables();
  magics::PopulateBishopTables();
  masks::init();

  for (std::string cmd = "greet"; REPL(game, exacto, cmd); std::cin >> cmd) {}

  return 0;
}
