#include "game.h"

#include <cstring>
#include <iostream>
#include <string>

#include "board.h"
#include "inlines.h"
#include "moves.h"
#include "squares.h"
#include "masks.h"
#include "zobrist_keys.h"

namespace exacto {

Game::Game(const std::string& brd,
           const std::string& clr,
           const std::string& cstl,
           const std::string& ep,
           const std::string& hm,
           const std::string& fm) {
  SetGame(brd, clr, cstl, ep, hm, fm);
}

void Game::SetGame(const std::string& brd,
                   const std::string& clr,
                   const std::string& cstl,
                   const std::string& ep,
                   const std::string& hm,
                   const std::string& fm) {
  SetBoard(brd, clr, cstl, ep);

  memset(half_moves, 0, sizeof(half_moves[0]) * STACK_SIZE);
  memset(position_history, 0, sizeof(position_history[0]) * STACK_SIZE);
  memset(repitition_hash, 0, sizeof(repitition_hash[0]) * REPITITION_HASH_SIZE);

  move_number = (atoi(fm.c_str()) - 1) * 2 + !wtm;
  half_moves[move_number] = atoi(hm.c_str());
  repitition_hash[hash_key >> REPITITION_HASH_SHIFT] = 1;
  position_history[move_number] = hash_key;
}

// operator== for comparing Games
bool Game::operator==(const Game &other) const {
  bool boards_equal = Board::operator==(other);
  bool move_data_equal = (move_number == other.move_number);
  bool position_history_equal = (move_number == other.move_number);
  for (int i = 0; i <= move_number; ++i) {
    move_data_equal &= (half_moves[i] == other.half_moves[i]);
    position_history_equal &=
       (position_history[i] == other.position_history[i]);
  }
  position_history_equal &=
      memcmp(repitition_hash, other.repitition_hash,
             sizeof(repitition_hash[0]) * REPITITION_HASH_SIZE) == 0;
  return boards_equal && move_data_equal && position_history_equal;
}

// Makes a move, fully updating the gamestate and history.
void Game::MakeMove(Move *m) {
  ind source = moves::source(*m);
  ind dest = moves::dest(*m);
  ind attacker = moves::attacker(*m);
  ind defender = moves::defender(*m);
  ind special = moves::special(*m);
  Bitboard source_bb = exp_2(source);
  Bitboard dest_bb = exp_2(dest);

  // Move the actual piece
  if (defender && special != EN_PASSANT_CAP) {
    KillPiece(!wtm, defender, dest, dest_bb);
  }
  MovePiece(wtm, attacker, source, dest, source_bb, dest_bb);

  // Set en passant square to 'none'
  Move en_passant_file = 9;
  if (en_passant) {
    en_passant_file = squares::file(bitscan(en_passant));
    set_en_passant();
  }
  *m |= en_passant_file << 18;

  // Special move stuff
  switch (special) {
    case EN_PASSANT_CAP: {
      ind hanging_pawn = wtm ? dest - 8 : dest + 8;
      KillPiece(!wtm, PAWN, hanging_pawn, exp_2(hanging_pawn));
      break;
    }
    case DOUBLE_PAWN_MOVE_W:
      set_en_passant(dest - 8);
      break;
    case DOUBLE_PAWN_MOVE_B:
      set_en_passant(dest + 8);
      break;
    case CASTLE: {
      ind rook_source = dest > source ? dest + 2 : dest - 1;
      ind rook_dest = dest > source ? dest - 1 : dest + 1;
      MovePiece(wtm, ROOK, rook_source, rook_dest, exp_2(rook_source),
                exp_2(rook_dest));
    }
    case KING_MOVE: {
      if (castling[wtm] & masks::FILE[1]) {
        RemoveKingsideCastlingRights(wtm);
        moves::EncodeKingsideCastlingChange(m, wtm);
      }
      if (castling[wtm] & masks::FILE[5]) {
        RemoveQueensideCastlingRights(wtm);
        moves::EncodeQueensideCastlingChange(m, wtm);
      }
      break;
    }
    case PROMOTE_QUEEN:
    case PROMOTE_ROOK:
    case PROMOTE_BISHOP:
    case PROMOTE_KNIGHT:
      KillPiece(wtm, PAWN, dest, dest_bb);
      MakePiece(wtm, special, dest, dest_bb);
      break;
  }

  // Remove castling rights for rook moves
  Bitboard castlingBB = castling[wtm];
  if (castlingBB != 0 && attacker == ROOK) {
    Bitboard kingSide = castlingBB & masks::FILE[1];
    if ((kingSide >> 1) & source_bb) {
      RemoveKingsideCastlingRights(wtm);
      moves::EncodeKingsideCastlingChange(m, wtm);
    }
    Bitboard queenSide = castlingBB & masks::FILE[5];
    if ((queenSide << 2) & source_bb) {
      RemoveQueensideCastlingRights(wtm);
      moves::EncodeQueensideCastlingChange(m, wtm);
    }
  }

  // Or for captures of a rook
  if (defender == ROOK && castling[!wtm] != 0) {
    if (wtm) {
      if (dest == H8 && (castling[BLACK] & exp_2(G8)) != 0) {
        RemoveKingsideCastlingRights(BLACK);
        moves::EncodeKingsideCastlingChange(m, BLACK);
      }
      if (dest == A8 && (castling[BLACK] & exp_2(C8)) != 0) {
        RemoveQueensideCastlingRights(BLACK);
        moves::EncodeQueensideCastlingChange(m, BLACK);
      }
    } else {
      if (dest == H1 && (castling[WHITE] & exp_2(G1)) != 0) {
        RemoveKingsideCastlingRights(WHITE);
        moves::EncodeKingsideCastlingChange(m, WHITE);
      }
      if (dest == A1 && (castling[WHITE] & exp_2(C1)) != 0) {
        RemoveQueensideCastlingRights(WHITE);
        moves::EncodeQueensideCastlingChange(m, WHITE);
      }
    }
  }

  wtm = !wtm;
  hash_key ^= zobrist::wtm;
  occupied = pieces[WHITE][ALL] | pieces[BLACK][ALL];

  // Increment half-move clock
  move_number++;
  half_moves[move_number] = half_moves[move_number - 1] + 1;
  if (defender || (attacker == PAWN)) {
    half_moves[move_number] = 0;
  }

  // Add new position to the position history and repition hash.
  position_history[move_number] = hash_key;
  repitition_hash[hash_key >> REPITITION_HASH_SHIFT]++;
}

// Fully undoes a move, updating gamestate and history accordingly.
void Game::UnmakeMove(Move m) {
  ind source = moves::source(m);
  ind dest = moves::dest(m);
  ind attacker = moves::attacker(m);
  ind special = moves::special(m);
  ind defender = moves::defender(m);
  Bitboard source_bb = exp_2(source);
  Bitboard dest_bb = exp_2(dest);

  //  Decrement half-move clock and reduce repitition hash sum.
  repitition_hash[hash_key >> REPITITION_HASH_SHIFT]--;
  move_number--;

  hash_key ^= zobrist::wtm;
  wtm = !wtm;

  // Special move stuff
  switch (special) {
    case EN_PASSANT_CAP: {
      ind hanging_pawn = wtm ? dest - 8 : dest + 8;
      MakePiece(!wtm, PAWN, hanging_pawn, exp_2(hanging_pawn));
      break;
    }
    case CASTLE: {
      ind rook_source = dest > source ? dest + 2 : dest - 1;
      ind rook_dest = dest > source ? dest - 1 : dest + 1;
      MovePiece(wtm, ROOK, rook_dest, rook_source, exp_2(rook_dest),
                exp_2(rook_source));
      break;
    }
    case PROMOTE_QUEEN:
    case PROMOTE_ROOK:
    case PROMOTE_BISHOP:
    case PROMOTE_KNIGHT:
      KillPiece(wtm, special, dest, dest_bb);
      MakePiece(wtm, PAWN, dest, dest_bb);
      break;
  }

  // Move the actual piece
  MovePiece(wtm, attacker, dest, source, dest_bb, source_bb);
  if (defender && special != EN_PASSANT_CAP) {
    MakePiece(!wtm, defender, dest, dest_bb);
  }

  // Restore castling rights
  ind castling_data = moves::castling(m);
  if (castling_data & 1) GrantKingsideCastlingRights(WHITE);
  if (castling_data & 2) GrantQueensideCastlingRights(WHITE);
  if (castling_data & 4) GrantKingsideCastlingRights(BLACK);
  if (castling_data & 8) GrantQueensideCastlingRights(BLACK);

  // Reset en passant square
  ind en_passant_file = moves::en_passant(m);
  if (en_passant_file < 9) {
    ind en_passant_rank = wtm ? 5 : 2;
    set_en_passant(8 * en_passant_rank + en_passant_file);
  } else if (en_passant) {
    set_en_passant();
  }

  occupied = pieces[WHITE][ALL] | pieces[BLACK][ALL];
}

// Browses the game tree and counts the number of nodes
Bitboard perft(Game *game, int depth) {
  Bitboard nodes = 1;

  if (depth > 0) {
    Move moves[256];
    game->MoveGen(moves);
    for (Move *move = moves; *move; ++move) {
      game->MakeMove(move);
      nodes += perft(game, depth - 1);
      game->UnmakeMove(*move);
    }
  }

  return nodes;
}

// Browse game tree and count nodes, list descendent counts for each child
Bitboard divide(Game *game, int depth) {
  Bitboard nodes = 0;
  Bitboard descendents = 0;

  Move moves[256];
  int number_of_moves = 0;
  game->MoveGen(moves);
  for (Move *move = moves; *move; ++move) {
    number_of_moves++;
    game->MakeMove(move);
    std::cout << moves::algebraic(*move) << " ";
    if (depth >= 2) {
      descendents = perft(game, depth - 1) - perft(game, depth - 2);
    } else {
      descendents = 1;
    }
    game->UnmakeMove(*move);
    std::cout << descendents << std::endl;
    nodes += descendents;
  }

  std::cout << "total " << nodes << std::endl;
  std::cout << "moves " << number_of_moves << std::endl;

  return nodes;
}

}  // namespace exacto
