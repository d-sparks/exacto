#include "exacto.h"

#include "board.h"
#include "game.h"

namespace exacto {

// search is an implementation of PVS ("principle variation search") with
// various pruning heuristics:
//   - Transposition table pruning
//   - TODO: many more
int16_t Exacto::Search(Game* game, int16_t alpha, int16_t beta, int16_t depth,
                        int16_t ply) {
  // Time management
  nodes++;

  // Transposition table pruning.
  uint8_t hashLookup = hash.probe(game->hash_key, depth);
  if (hashLookup) {
    int16_t hashVal = hash.get_val(game->hash_key);
    if ((hashLookup == HASH_EXACT) ||
        (hashLookup == HASH_BETA && hashVal >= beta) ||
        (hashLookup == HASH_ALPHA && hashVal < alpha)) {
      return hashVal;
    }
  }

  // Check for a draw by repition or 50 move rule.
  if (drawn_by_repition_or_50_move_rule(game)) {
    return DRAWSCORE;
  }

  // At depth = 0, call qsearch to continue searching exciting moves.
  if (depth == 0) {
    return QSearch(game, alpha, beta, ply);
  }

  // Generate legal moves to determine children nodes.
  Move Moves[256] = {0};
  int bestScore = alpha;
  Move bestMove = BOGUS_MOVE;
  game->MoveGen(Moves);

  // If there are no legal moves, this node is either checkmate or stalemate.
  if (Moves[0] == NONE) {
    return game->in_check() ? -MATESCORE + ply : DRAWSCORE;
  }

  // Sort the moves
  SortMoves(game, Moves);

#ifdef _DEBUG
  Game reference = *game;
#endif

  // PVS algorithm: iterate over each child, recurse.
  for (ind i = 0; Moves[i]; i++) {
    Move move = Moves[i];
    game->makeMove(&move);
    int score = -Search(game, -beta, -bestScore, depth - 1, ply + 1);
    game->unmakeMove(move);

#ifdef _DEBUG
    if (!(*game == reference)) {
      game->print();
      cout << "Reference: " << endl;
      reference.print();
      for (ind j = 0; Moves[j]; j++) {
        cout << moves::algebraic(Moves[j]) << endl;
      }
      throw 1;
    }
#endif

    // If score >= beta, the opponent has a better move than the one they
    // played, so we can stop searching. We note in the transposition table that
    // we only have a lower bound on the score of this node. (Fail high.)
    if (score >= beta) {
      hash.record(game->hash_key, bestMove, depth, HASH_BETA, score);
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
    hash.record(game->hash_key, bestMove, depth, HASH_ALPHA, bestScore);
    return alpha;
  }

  // Record the result in the hash table and return the score.
  hash.record(game->hash_key, bestMove, depth, HASH_EXACT, bestScore);
  return bestScore;
}

// qsearch ("quiescence search") is alphabeta except that only exciting or
// "loud" nodes are traversed, things like checks, pawn promotions and captures.
// In the event of being in check, all evasions are searched.
int16_t Exacto::QSearch(Game* game, int16_t alpha, int16_t beta,
                         int16_t ply) {
  // Time control
  nodes++;

  // Check for a draw by repition or 50 move rule.
  if (drawn_by_repition_or_50_move_rule(game)) {
    return DRAWSCORE;
  }

  int16_t score = Evaluate(game);
  if (score >= beta) {
    return score;
  }
  if (alpha < score) {
    alpha = score;
  }

  // If in check, search all evasions. Otherwise, only loud moves.
  Move Moves[256] = {0};
  if (game->in_check()) {
    game->MoveGen(Moves);
    // If no evasions exist, this node is checkmate.
    if (Moves[0] == NONE) {
      return -MATESCORE + ply;
    }
  } else {
    game->CapGen(Moves);
  }

  // Sort moves
  SortCaps(game, Moves);

  // This is basic alphabeta.
  for (ind i = 0; Moves[i]; i++) {
    Move move = Moves[i];
    game->makeMove(&move);
    int score = -QSearch(game, -beta, -alpha, ply + 1);
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

}  // namespace exacto
