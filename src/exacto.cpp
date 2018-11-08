#include "exacto.h"

#include <iostream>
#include <string>

#include "board.h"
#include "game.h"
#include "hash.h"
#include "moves.h"
#include "SEE.h"

namespace exacto {

Exacto::Exacto(Game initGame) {
  game = initGame;
  post = false;
#ifndef _DEBUG
  post = true;
#endif
}

Exacto::~Exacto() {}

void Exacto::Go(Game* game) {
  // hardcoding checks per second and average time for search
  int seconds = 7;

  // timing initialization
  nodes = 0;
  uint64_t prev_nodes = 1;
  double branching_factor = 3;
  int NPS = 1000000;
  terminate_search = false;
  int64_t t_0 = clock();
  int64_t t = 0;

  Move best_move = BOGUS_MOVE;
  for (int depth = 1; true; ++depth) {
    prev_nodes = nodes;
    int16_t score = Search(game, -INFNTY, INFNTY, depth, 0);
    if (depth == 1) {
      continue;
    }

    t = clock() - t_0;
    double secs = double(t) / CLOCKS_PER_SEC;
    branching_factor = nodes / prev_nodes;
    NPS = (nodes * CLOCKS_PER_SEC) / t;

    if (terminate_search) {
      break;
    }

    best_move = hash.get_sugg(game->hash_key);
    if (post) {
      std::cout << depth << "\t";
      std::cout << "Score: " << score << "\t";
      std::cout << "PV: " << principal_variation(game, depth) << "\t";
      std::cout << "NPS: " << NPS << "\t";
      std::cout << "Branching: " << branching_factor << std::endl;
    }

    if (secs * branching_factor > seconds) {
      break;
    }
  }

#ifndef _DEBUG
  std::cout << "move " << moves::algebraic(best_move) << std::endl;
#endif
#ifdef _DEBUG
  std::cout << " usermove " << moves::algebraic(best_move);
#endif
  game->MakeMove(&best_move);
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
  int half_moves = game->half_moves[game->move_number];
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

void Exacto::set_time(int time) {
  time_manager.set_time(time);
}

void Exacto::set_opponent_time(int opponent_time) {
  time_manager.set_opponent_time(opponent_time);
}

}  // namespace exacto
