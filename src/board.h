#ifndef exacto_src_board_h
#define exacto_src_board_h

#include <string>
#include <vector>

#include "bitboard.h"
#include "moves.h"

namespace exacto {

// Board contains the state of the board. It's main method is MoveGen, so, in
// principle, it should  have just enough information to generate the legal
// moves. It does not contain peripheral information like times, moves.history,
// etc.
class Board {
 public:
  Board(const std::string& brd = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR",
        const std::string& clr = "w",
        const std::string& cstl = "KQkq",
        const std::string& ep = "-");

  void SetBoard(
      const std::string& brd = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR",
      const std::string& clr = "w",
      const std::string& cstl = "KQkq",
      const std::string& ep = "-");

  void MoveGen(Move *move_list);
  void CapGen(Move *move_list);

  void MoveGen(Move *move_list, bool quiet_moves);

  void MovePiece(bool color,
                 ind attacker,
                 ind source,
                 ind dest,
                 Bitboard source_bb,Bitboard dest_bb);
  void MakePiece(bool color, ind piece, ind square, Bitboard square_bb);
  void KillPiece(bool color, ind piece, ind square, Bitboard square_bb);

  void RemoveQueensideCastlingRights(bool color);
  void RemoveKingsideCastlingRights(bool color);
  void GrantQueensideCastlingRights(bool color);
  void GrantKingsideCastlingRights(bool color);

  void set_en_passant(ind square = 64);
  bool in_check();
  bool have_piece();

  bool operator==(const Board &other) const;

  // For debugging
  void Print();
  void Print(const std::vector<std::string>& side_bar);

  // The state of the board.
  uint64_t hash_key;
  bool wtm;
  Bitboard pieces[2][7];
  Bitboard castling[2];
  Bitboard en_passant;
  Bitboard occupied;
  Bitboard empty;
  ind board[64];

#ifndef _TEST
 protected:
#endif
  // Move generation helpers functions. Despite being protected, these methods
  // are unit tested due to their complexity.
  void PawnGen(Move **move_list, Bitboard pins, bool quiet_moves);
  void PawnCaps(Move **move_list, Bitboard pins);
  void PawnGenPinned(Move **move_list,
                     Bitboard pins,
                     ind king_square,
                     bool quiet_moves);
  void KnightGen(Move **move_list, Bitboard pins, bool quiet_moves);
  void BishopGen(Move **move_list, Bitboard pins, bool quiet_moves);
  void BishopGenPinned(Move **move_list,
                       Bitboard pins,
                       ind king_square,
                       bool quiet_moves);
  void RookGen(Move **move_list, Bitboard pins, bool quiet_moves);
  void RookGenPinned(Move **move_list,
                     Bitboard pins,
                     ind king_square,
                     bool quiet_moves);
  void KingGen(Move **move_list,
               ind king_square,
               Bitboard enemey_attacks,
               bool quiet_moves);

  void GenerateMovesTo(Move **move_list,
                       ind square,
                       ind defender,
                       Bitboard pins,
                       Bitboard enemey_attacks);
  void EvasionGen(Move **move_list,
                  Bitboard enemey_attacks,
                  Bitboard pins,
                  ind king_square);
  Bitboard AttackSetGen(bool color);

  Bitboard BishopPins(ind king_square);
  Bitboard RookPins(ind king_square);

  void Serialize(Move **move_list, Bitboard b, ind source);
  void Serialize(Move **move_list, Bitboard b, ind source, ind special);
  void SerializeFromDest(Move **move_list,
                         Bitboard b,
                         ind dest,
                         ind defender,
                         ind special);

  void SerializePawn(Move **move_list, Bitboard b, ind special, int delta);
  void CloseMoveList(Move **move_list);

};

}  // namespace exacto

#endif  // exacto_src_board_h
