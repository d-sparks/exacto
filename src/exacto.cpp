#include "exacto.h"

#include <iostream>
#include <string>

#include "board.h"
#include "game.h"
#include "hash.h"
#include "inlines.h"
#include "magics.h"
#include "masks.h"
#include "moves.h"
#include "SEE.h"
#include "squares.h"

namespace exacto {

Exacto::Exacto() {
  magics::PopulateRookTables();
  magics::PopulateBishopTables();
  masks::init();

  post = false;
  force = false;
}

Exacto::~Exacto() {}

void Exacto::Go(Game* game) {
  Move best_move = FindMove(game);
#ifndef _DEBUG
  std::cout << "move " << moves::algebraic(best_move) << std::endl;
#endif
#ifdef _DEBUG
  std::cout << " usermove " << moves::algebraic(best_move);
#endif
  game->MakeMove(&best_move);
}

Move Exacto::FindMove(Game* game, Move target_move) {
  force = false;

  // hardcoding checks per second and average time for search
  int ideal_time;
  int maximum_time;
  time_manager.GetTimeForMove(game->full_move_number(), &ideal_time,
                              &maximum_time);

  // timing initialization
  search_info.nodes = 0;
  uint64_t prev_nodes = 1;
  search_info.branching_factor = 3;
  search_info.nodes_per_second = 1000000;
  terminate_search = false;
  int64_t t_0 = clock();
  int64_t t = 0;
  time_manager.nodes_next_clock_check = NODES_PER_CLOCK_CHECK;
  time_manager.max_clock = t_0 + maximum_time * CLOCKS_PER_SEC / 100;

  Move best_move = BOGUS_MOVE;
  for (int depth = 1; true; ++depth) {
    prev_nodes = search_info.nodes;
    int16_t score = Search(game, -INFNTY, INFNTY, depth, 0);
    if (depth == 1) {
      continue;
    }

    t = clock() - t_0;

    search_info.branching_factor = (float)search_info.nodes / prev_nodes;
    search_info.time_used = 100 * (float)t / CLOCKS_PER_SEC;
    search_info.nodes_per_second =
      (float)search_info.nodes * CLOCKS_PER_SEC / t;
    search_info.score = score;
    search_info.depth = depth;

    if (terminate_search) {
      break;
    }

    best_move = hash.get_sugg(game->hash_key);
    if (post) {
      std::cout << search_info.depth << "\t" << search_info.score << "\t"
                << (int)search_info.time_used << "\t" << search_info.nodes
                << "\t" << principal_variation(game, depth) << std::endl;
    }

    if (target_move != BOGUS_MOVE && best_move == target_move) {
      break;
    }

    int estimate = search_info.time_used * search_info.branching_factor;
    int high_estimate = 1.5 * estimate;
    if (!time_manager.use_exact_time && (high_estimate > maximum_time ||
                                         estimate > ideal_time)) {
      break;
    }
  }

  return best_move;
}

void Exacto::SortMoves(Game* game, Move* moves) {
  Move hash_sugg = BOGUS_MOVE;
  if (hash.probe(game->hash_key, 0) > HASH_ALPHA) {
    hash_sugg = hash.get_sugg(game->hash_key);
  }
  ind num_moves = 0;
  while (moves[num_moves] != NONE) {
    num_moves++;
  }
  int16_t scores[num_moves];
  for (int i = 0; i < num_moves; ++i) {
    Move move = moves[i];
    int16_t score = (move == hash_sugg) ? MATESCORE : SEE::see(game, move);
    ind attacker = moves::attacker(move);
    if (attacker <= BISHOP) {
      ind source = moves::source(move);
      ind dest = moves::dest(move);
      if (game->wtm) {
        score += PVT[WHITE][attacker][dest] - PVT[WHITE][attacker][source];
      } else {
        score -= PVT[BLACK][attacker][dest] - PVT[BLACK][attacker][source];
      }
    }
    int j = i;
    for (; j > 0 && score > scores[j - 1]; --j) {
      scores[j] = scores[j - 1];
      moves[j] = moves[j - 1];
    }
    moves[j] = move;
    scores[j] = score;
  }
}

void Exacto::SortCaps(Game* game, Move* moves) {
  int num_moves = 0;
  while (moves[num_moves]) {
    num_moves++;
  }
  int16_t scores[num_moves];
  for (int i = 0; i < num_moves; ++i) {
    Move move = moves[i];
    int16_t score = SEE::see(game, move);
    int j = i;
    for (; j > 0 && score > scores[j - 1]; --j) {
      scores[j] = scores[j - 1];
      moves[j] = moves[j - 1];
    }
    moves[j] = move;
    scores[j] = score;
  }
}

bool Exacto::drawn_by_repitition_or_50_move_rule(Game* game) {
  int half_moves = game->half_moves();
  if (half_moves >= 100) {
    return true;
  }

  if (game->repitition_hash[game->hash_key >> REPITITION_HASH_SHIFT] < 3 ||
      half_moves < 4) {
    return false;
  }

  int count = 0;
  for (int i = game->move_number - half_moves; i < game->move_number; ++i) {
    if (game->position_history[i] == game->hash_key) {
      count++;
    }
    if (count >= 2) {
      return true;
    }
  }

  return false;
}

std::string Exacto::principal_variation(Game* game, int depth) {
  // At depth 0 or if no hash entry, return.
  if (depth == 0 || hash.probe(game->hash_key, depth) != HASH_EXACT) {
    return "";
  }

  // Check if hashed suggestion is legal
  Move hash_sugg = hash.get_sugg(game->hash_key);
  Move pv_move = hash.get_pv(game->hash_key);
  bool hash_legal = false;
  bool pv_legal = false;
  Move legal_moves[256] = {0};
  game->MoveGen(legal_moves);
  for (int i = 0; legal_moves[i]; ++i) {
    hash_legal |=
        (moves::algebraic(hash_sugg) == moves::algebraic(legal_moves[i]));
    hash_legal |=
        (moves::algebraic(pv_move) == moves::algebraic(legal_moves[i]));
  }
  if (!hash_legal && !pv_legal) {
    return "";
  }

  // Recurse
  Move move = pv_legal? pv_move : hash_sugg;
  std::string output = moves::algebraic(move);
  game->MakeMove(&move);
  output += " " + principal_variation(game, depth - 1);
  game->UnmakeMove(move);

  return output;
}

void Exacto::SetLevels(int MPS, int base_time, int increment) {
  time_manager.SetLevels(MPS, base_time, increment);
}

void Exacto::SetTime(int centiseconds, bool exact, bool opponent) {
  if (opponent) {
    time_manager.opponent_time = centiseconds;
  } else {
    time_manager.time = centiseconds;
  }
  time_manager.use_exact_time = exact;
}

void Exacto::Print(Game* game) {
  constexpr int debug_width = 15;
  std::vector<std::string> debug;
  auto add_str_debug = [&](const std::string& k, const std::string& v) {
    debug.push_back(k);
    while (debug.back().length() < debug_width) debug.back() += ' ';
    debug.back() += v;
  };
  auto add_int_debug = [&](const std::string& k, int v) {
    add_str_debug(k, std::to_string(v));
  };

  // Make castling string
  std::string castling_string = "";
  if (game->castling[WHITE] & exp_2(G1)) castling_string += (std::string)"K";
  if (game->castling[WHITE] & exp_2(C1)) castling_string += (std::string)"Q";
  if (game->castling[BLACK] & exp_2(G8)) castling_string += (std::string)"k";
  if (game->castling[BLACK] & exp_2(G8)) castling_string += (std::string)"q";
  if (castling_string.empty()) castling_string += "-";

  // Make time control string
  std::string time_control = std::to_string(time_manager.MPS) + "/" +
                             std::to_string(time_manager.base_time) + "/" +
                             std::to_string(time_manager.increment);

  debug.push_back("--Board--");
  add_str_debug("Castling:", castling_string);
  add_str_debug("To move:", (game->wtm ? "W" : "B"));

  debug.push_back("--Game--");
  add_int_debug("Half moves:", game->half_moves());
  add_int_debug("Full moves:", game->full_move_number());
  add_int_debug("Repititions:", game->repitition_ub());

  debug.push_back("--Exacto--");
  add_int_debug("Static eval:", Evaluate(game));
  add_int_debug("QSearch eval:", QSearch(game, -MATESCORE, MATESCORE, 0));
  add_int_debug("Time:", time_manager.time);
  add_int_debug("Opponent time:", time_manager.opponent_time);
  add_str_debug("Control", time_control);

  game->Print(debug);
}

}  // namespace exacto
