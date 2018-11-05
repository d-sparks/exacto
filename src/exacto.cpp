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
  uint64_t prevNodes = 1;
  double branchingFactor = 3;
  int NPS = 1000000;
  terminate_search = false;
  int64_t t_0 = clock();
  int64_t t = 0;

  Move bestMove = BOGUS_MOVE;
  for (int depth = 1; true; depth++) {
    prevNodes = nodes;
    int16_t score = Search(game, -INFNTY, INFNTY, depth, 0);
    if (depth == 1) {
      continue;
    }

    t = clock() - t_0;
    double secs = double(t) / CLOCKS_PER_SEC;
    branchingFactor = nodes / prevNodes;
    NPS = (nodes * CLOCKS_PER_SEC) / t;

    if (terminate_search) {
      break;
    }

    bestMove = hash.get_sugg(game->hash_key);
    if (post) {
      std::cout << depth << "\t";
      std::cout << "Score: " << score << "\t";
      std::cout << "PV: " << principal_variation(game, depth) << "\t";
      std::cout << "NPS: " << NPS << "\t";
      std::cout << "Branching: " << branchingFactor << std::endl;
    }

    if (secs * branchingFactor > seconds) {
      break;
    }
  }

#ifndef _DEBUG
  std::cout << "move " << moves::algebraic(bestMove) << std::endl;
#endif
#ifdef _DEBUG
  std::cout << " usermove " << moves::algebraic(bestMove);
#endif
  game->makeMove(&bestMove);
}

void Exacto::SortMoves(Game* game, Move* Moves) {
  Move hashSugg = BOGUS_MOVE;
  if (hash.probe(game->hash_key, 0) > HASH_ALPHA) {
    hashSugg = hash.get_sugg(game->hash_key);
  }
  ind numMoves = 0;
  while (Moves[numMoves] != NONE) {
    numMoves++;
  }
  int16_t scores[numMoves];
  for (int i = 0; i < numMoves; i++) {
    Move move = Moves[i];
    int16_t score = (move == hashSugg) ? MATESCORE : SEE::see(game, move);
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
    for (; j > 0 && score > scores[j - 1]; j--) {
      scores[j] = scores[j - 1];
      Moves[j] = Moves[j - 1];
    }
    Moves[j] = move;
    scores[j] = score;
  }
}

void Exacto::SortCaps(Game* game, Move* moves) {
  int numMoves = 0;
  while (moves[numMoves]) {
    numMoves++;
  }
  int16_t scores[numMoves];
  for (int i = 0; i < numMoves; i++) {
    Move move = moves[i];
    int16_t score = SEE::see(game, move);
    int j = i;
    for (; j > 0 && score > scores[j - 1]; j--) {
      scores[j] = scores[j - 1];
      moves[j] = moves[j - 1];
    }
    moves[j] = move;
    scores[j] = score;
  }
}

std::string Exacto::principal_variation(Game* game, int depth) {
  // At depth 0 or if no hash entry, return.
  if (depth == 0 || hash.probe(game->hash_key, depth) != HASH_EXACT) {
    return "";
  }

  // Check if hashed suggestion is legal
  Move hashMove = hash.get_sugg(game->hash_key);
  Move PVMove = hash.get_pv(game->hash_key);
  bool hashLegal = false;
  bool PVLegal = false;
  Move legalMoves[256] = {0};
  game->MoveGen(legalMoves);
  for (int i = 0; legalMoves[i]; ++i) {
    hashLegal |= (moves::algebraic(hashMove) == moves::algebraic(legalMoves[i]));
    hashLegal |= (moves::algebraic(PVMove) == moves::algebraic(legalMoves[i]));
  }
  if (!hashLegal && !PVLegal) {
    return "";
  }

  // Recurse
  Move move = PVLegal? PVMove : hashMove;
  std::string output = moves::algebraic(move);
  game->makeMove(&move);
  output += " " + principal_variation(game, depth - 1);
  game->unmakeMove(move);

  return output;
}

}  // namespace exacto
