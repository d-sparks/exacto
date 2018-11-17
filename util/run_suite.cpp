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

  std::string am;
  std::string bm;

  std::string epd;
};

// Format assumption:
// FEN key val; key val; key val;
bool ReadEpd(std::ifstream& file, EPD* output) {
  std::string line;
  if (!std::getline(file, line)) {
    return false;
  }

  output->epd = line;
  std::istringstream iss(line);

  iss >> output->board;
  iss >> output->color;
  iss >> output->castling;
  iss >> output->en_passant;
  iss >> output->half_moves;
  iss >> output->full_moves;

  std::string k;
  std::string v;

  output->am = "bogus";
  output->bm = "bogus";

  while (iss >> k >> v) {
    if (v[v.length() - 1] == ';') v = v.substr(0, v.length() - 1);
    if (k == "id") {
      output->id = v;
    } else if (k == "am") {
      output->am = v;
    } else if (k == "bm") {
      output->bm = v;
    }
  }

  return output;
}

Move FindMoveOnGame(Game* game, std::string move) {
  Move move_list[256];
  game->MoveGen(move_list);
  for (int i = 0; move_list[i]; ++i) {
    if (moves::algebraic(move_list[i]) == move ||
        game->fancy_algebraic(move_list[i]) == move) {
      return move_list[i];
    }
  }
  return BOGUS_MOVE;
}

bool Equals(Move move, Move other) {
  if (move == BOGUS_MOVE || other == BOGUS_MOVE) return false;
  return move == other || moves::algebraic(move) == moves::algebraic(other);
}

}  // namespace

int main(int argc, char* argv[]) {
  Game game;
  Exacto exacto;

  // Use no more than 60 seconds
  exacto.SetTime(5 * 100, true, false);

  std::ifstream file(argv[ 1]);

  int passes = 0;
  int cases = 0;
  double sum_square_time = 0;

  EPD epd;
  std::vector<std::string> failures;
  while (ReadEpd(file, &epd)) {
    game.SetGame(epd.board, epd.color, epd.castling, epd.en_passant,
                 epd.half_moves, epd.full_moves);

    Move best = FindMoveOnGame(&game, epd.bm);
    Move avoid = FindMoveOnGame(&game, epd.am);
    Move move = exacto.FindMove(&game, best);

    bool pass = Equals(move, best) ||
                (best == BOGUS_MOVE && !Equals(move, avoid));
    passes += pass;
    if (!pass) failures.push_back(epd.epd);
    sum_square_time +=
        exacto.search_info.time_used * exacto.search_info.time_used;
    cases++;

    std::vector<std::string> debug;
    debug.push_back(pass ? "--PASS--" : "--FAIL--");
    debug.push_back("Best:   " + epd.bm);
    debug.push_back("Avoid:  " + epd.am);
    debug.push_back("Got:    " + game.fancy_algebraic(move));
    debug.push_back("Time:   " + std::to_string(exacto.search_info.time_used));

    game.Print(debug);

  }

  std::cout << std::endl << "--FAILURES--" << std::endl;
  for (const std::string& failure : failures) {
    std::cout << failure << std::endl;
  }

  std::cout << std::endl << "--RESULTS--" << std::endl;
  std::cout << "Pass:              " << passes << std::endl
            << "Total:             " << cases << std::endl
            << "Mean square time:  " << (sum_square_time / cases) << std::endl;

  return 0;
}
