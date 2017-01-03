#pragma once
#include "cexacto.h"
#include "cgame.h"

// search is an implementation is PVS ("principle variation search") with
// various pruning
// heuristics:
//   - Transposition table pruning
//   - TODO: many more
int16_t CExacto::search(CGame* game, int16_t alpha, int16_t beta, int16_t depth,
                        int16_t ply) {
  // Time management
  nodes++;

  // Transposition table pruning.
  uint8_t hashLookup = hash.probe(game->hashKey, depth);
  if (hashLookup) {
    int16_t hashVal = hash.getVal(game->hashKey);
    if (hashLookup == HASH_EXACT ||
        (hashLookup == HASH_BETA && hashVal >= beta) ||
        (hashLookup == HASH_ALPHA && hashVal < alpha)) {
      return hashVal;
    }
  }

  // Check for a draw by repition or 50 move rule.
  if (drawnByRepitionOr50MoveRule(game)) {
    return DRAWSCORE;
  }

  // At depth = 0, call qsearch to continue searching exciting moves.
  if (depth == 0) {
    return qsearch(game, alpha, beta, ply);
  }

  // Generate legal moves to determine children nodes.
  mv mvs[256] = {0};
  int bestScore = alpha;
  mv bestMove = BOGUS_MOVE;
  game->moveGen(mvs);

  // If there are no legal moves, this node is either checkmate or stalemate.
  if (mvs[0] == NONE) {
    return game->inCheck() ? -MATESCORE + ply : DRAWSCORE;
  }

  // Sort the moves
  sortMoves(game, mvs);

#ifdef _DEBUG
  CGame reference = *game;
#endif

  // PVS algorithm: iterate over each child, recurse.
  for (ind i = 0; mvs[i]; i++) {
    mv move = mvs[i];
    game->makeMove(&move);
    int score = -search(game, -beta, -bestScore, depth - 1, ply + 1);
    game->unmakeMove(move);

#ifdef _DEBUG
    if (!(*game == reference)) {
      cout << endl << "Game: " << endl;
      game->print();
      cout << "Reference: " << endl;
      reference.print();
      for (ind j = 0; mvs[j]; j++) {
        cout << moves::algebraic(mvs[j]) << endl;
      }
      throw 1;
    }
#endif

    // If score >= beta, the opponent has a better move than the one they
    // played, so we can stop searching. We note in the transposition table that
    // we only have a lower bound on the score of this node. (Fail high.)
    if (score >= beta) {
      hash.record(game->hashKey, bestMove, depth, score, HASH_BETA);
      return score;
    }

    // Keep track of the best move.
    if (score > bestScore) {
      bestScore = score;
      bestMove = move;
    }
  }

  // If bestScore == alpha, no move improved alpha, meaning we can't be sure a
  // descendent didn't fail high, so we note that we only know an upper bound on
  // the score for this node. (Fail low.)
  if (bestScore == alpha) {
    hash.record(game->hashKey, bestMove, depth, alpha, HASH_ALPHA);
    return alpha;
  }

  // Record the result in the hash table and return the score.
  hash.record(game->hashKey, bestMove, depth, bestScore, HASH_EXACT);
  return bestScore;
}

// qsearch ("quiescence search") is alphabeta except that only exciting or
// "loud" nodes are traversed, things like checks, pawn promotions and captures.
// In the event of being in check, all evasions are searched.
int16_t CExacto::qsearch(CGame* game, int16_t alpha, int16_t beta,
                         int16_t ply) {
  // Time control
  nodes++;

  // Check for a draw by repition or 50 move rule.
  if (drawnByRepitionOr50MoveRule(game)) {
    return DRAWSCORE;
  }

  int16_t score = evaluate(game);
  if (score >= beta) {
    return score;
  }
  if (alpha < score) {
    alpha = score;
  }

  // If in check, search all evasions. Otherwise, only loud moves.
  mv mvs[256] = {0};
  if (game->inCheck()) {
    game->moveGen(mvs);
    // If no evasions exist, this node is checkmate.
    if (mvs[0] == NONE) {
      return -MATESCORE + ply;
    }
  } else {
    game->capGen(mvs);
  }

  // Sort moves
  sortCaps(game, mvs);

  // This is basic alphabeta.
  for (ind i = 0; mvs[i]; i++) {
    mv move = mvs[i];
    game->makeMove(&move);
    int score = -qsearch(game, -beta, -alpha, ply + 1);
    game->unmakeMove(move);
    if (score >= beta) {
      return score;
    }
    if (score > alpha) {
      alpha = score;
    }
  }

  return alpha;
}
