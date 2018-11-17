#include "exacto.h"

#include "board.h"
#include "game.h"

namespace exacto {

// search is an implementation of PVS ("principle variation search") with
// various pruning heuristics:
//   - Transposition table pruning
//   - TODO: many more
int16_t Exacto::Search(Game* game,
                       int16_t alpha,
                       int16_t beta,
                       int16_t depth,
                       int16_t ply) {
  // Time management
  if (terminate_search) {
    return alpha;
  }
  search_info.nodes++;
  if (search_info.nodes >= time_manager.nodes_next_clock_check) {
    if (clock() >= time_manager.max_clock) {
      terminate_search = true;
      return alpha;
    } else {
      time_manager.nodes_next_clock_check =
          search_info.nodes + NODES_PER_CLOCK_CHECK;
    }
  }

  // Transposition table pruning.
  uint8_t hash_lookup = hash.probe(game->hash_key, depth);
  if (hash_lookup) {
    int16_t hash_value = hash.get_val(game->hash_key);
    if ((hash_lookup == HASH_EXACT) ||
        (hash_lookup == HASH_BETA && hash_value >= beta) ||
        (hash_lookup == HASH_ALPHA && hash_value < alpha)) {
      return hash_value;
    }
  }

  // Check for a draw by repition or 50 move rule.
  if (drawn_by_repitition_or_50_move_rule(game)) {
    return DRAWSCORE;
  }

  // At depth = 0, call qsearch to continue searching exciting moves.
  if (depth == 0) {
    return QSearch(game, alpha, beta, ply);
  }

  // Generate legal moves to determine children nodes.
  Move moves[256] = {0};
  int best_score = alpha;
  Move best_move = BOGUS_MOVE;
  game->MoveGen(moves);

  // If there are no legal moves, this node is either checkmate or stalemate.
  if (moves[0] == NONE) {
    return game->in_check() ? -MATESCORE + ply : DRAWSCORE;
  }

  // Sort the moves
  SortMoves(game, moves);

#ifdef _DEBUG
  Game reference = *game;
#endif

  // PVS algorithm: iterate over each child, recurse.
  for (ind i = 0; moves[i]; i++) {
    Move move = moves[i];
    game->MakeMove(&move);
    int score = -Search(game, -beta, -best_score, depth - 1, ply + 1);
    game->UnmakeMove(move);

#ifdef _DEBUG
    if (!(*game == reference)) {
      game->print();
      cout << "Reference: " << endl;
      reference.print();
      for (ind j = 0; moves[j]; j++) {
        cout << moves::algebraic(moves[j]) << endl;
      }
      throw 1;
    }
#endif

    // If score >= beta, the opponent has a better move than the one they
    // played, so we can stop searching. We note in the transposition table that
    // we only have a lower bound on the score of this node. (Fail high.)
    if (score >= beta && !terminate_search) {
      hash.record(game->hash_key, moves[i], depth, HASH_BETA, score);
      return score;
    }

    // Keep track of the best move.
    if (score > best_score) {
      best_score = score;
      best_move = moves[i];  // because moves[i] was mutated
    }
  }

  // If best_score == alpha, no move improved alpha, meaning we can't be sure a
  // descendent didn't fail high, so we note that we only know an upper bound on
  // the score for this node. (Fail low.)
  if (best_score == alpha) {
    hash.record(game->hash_key, best_move, depth, HASH_ALPHA, best_score);
    return alpha;
  }

  // Record the result in the hash table and return the score.
  hash.record(game->hash_key, best_move, depth, HASH_EXACT, best_score);
  return best_score;
}

// qsearch ("quiescence search") is alphabeta except that only exciting or
// "loud" nodes are traversed, things like checks, pawn promotions and captures.
// In the event of being in check, all evasions are searched.
int16_t Exacto::QSearch(Game* game,
                        int16_t alpha,
                        int16_t beta,
                        int16_t ply) {
  // Time control
  search_info.nodes++;

  // Check for a draw by repition or 50 move rule.
  if (drawn_by_repitition_or_50_move_rule(game)) {
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
  Move moves[256] = {0};
  if (game->in_check()) {
    game->MoveGen(moves);
    // If no evasions exist, this node is checkmate.
    if (moves[0] == NONE) {
      return -MATESCORE + ply;
    }
  } else {
    game->CapGen(moves);
  }

  // Sort moves
  SortCaps(game, moves);

  // This is basic alphabeta.
  for (ind i = 0; moves[i]; i++) {
    Move move = moves[i];
    game->MakeMove(&move);
    int score = -QSearch(game, -beta, -alpha, ply + 1);
    game->UnmakeMove(move);
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
