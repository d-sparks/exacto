#include "../src/exacto.h"
#include "../src/game.h"
#include "../src/moves.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

using namespace exacto;

namespace {

struct EPD {
  std::string id;

  std::string board;
  std::string color;
  std::string castling;
  std::string en_passant;
  std::string half_moves;
  std::string full_moves;

  std::string opcode;
  std::string move;
};

// Format assumption:
// FEN key val; key val; key val;
bool read_epd(std::ifstream& file, EPD* output) {
  std::string line;
  if (!std::getline(file, line)) {
    return false;
  }

  std::istringstream iss(line);

  iss >> output->board;
  iss >> output->color;
  iss >> output->castling;
  iss >> output->en_passant;
  iss >> output->half_moves;
  iss >> output->full_moves;

  std::string k;
  std::string v;

  while (iss >> k >> v) {
    if (v[v.length() - 1] == ';') v = v.substr(0, v.length() - 1);
    if (k == "id") {
      output->id = v;
    } else {
      output->opcode = k;
      output->move = v;
    }
  }

  return output;
}

}  // namespace

int main(int argc, char* argv[]) {
  Game game;
  Exacto exacto;

  std::ifstream file(argv[1]);
  EPD epd;

  int pass = 0;
  int fail = 0;

  while (read_epd(file, &epd)) {
    game.SetGame(epd.board, epd.color, epd.castling, epd.en_passant,
                 epd.half_moves, epd.full_moves);
    Move move = exacto.FindMove(&game);
    auto expected = epd.move;
    auto actual = game.fancy_algebraic(move);
    pass += (expected == actual);
    fail += (expected != actual);

    std::vector<std::string> debug;
    debug.push_back((expected == actual) ? "PASS" : "FAIL");
    debug.push_back("Expected:\t" + expected);
    debug.push_back("Got:\t\t" + actual);

    game.Print(debug);
  }

  std::cout << "Result: " << pass << " of " << (pass + fail) << " pass."
            << std::endl;

  return 0;
}
