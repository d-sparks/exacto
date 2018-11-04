#ifndef exacto_src_board_h
#define exacto_src_board_h

#include <string>

#include "bitboard.h"
#include "moves.h"

namespace exacto {

// Board contains the state of the board. It's main method is MoveGen, so, in
// principle, it should  have just enough information to generate the legal
// moves. It does not contain peripheral information like times, moves.history,
// etc.
class Board {
 public:
  Board(std::string brd = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR",
        std::string clr = "w",
        std::string cstl = "KQkq",
        std::string ep = "-");

  void setBoard(std::string brd = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR",
                std::string clr = "w",
                std::string cstl = "KQkq",
                std::string ep = "-");

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

  bool operator==(const Board &other) const;

  // For debugging
  void print();

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
  void pawnGen(Move **move_list, Bitboard pins, bool quiet_moves);
  void pawnCaps(Move **move_list, Bitboard pins);
  void pawnGenPinned(Move **move_list,
                     Bitboard pins,
                     ind king_square,
                     bool quiet_moves);
  void knightGen(Move **move_list, Bitboard pins, bool quiet_moves);
  void bishopGen(Move **move_list, Bitboard pins, bool quiet_moves);
  void bishopGenPinned(Move **move_list,
                       Bitboard pins,
                       ind king_square,
                       bool quiet_moves);
  void rookGen(Move **move_list, Bitboard pins, bool quiet_moves);
  void rookGenPinned(Move **move_list,
                     Bitboard pins,
                     ind king_square,
                     bool quiet_moves);
  void kingGen(Move **move_list,
               ind king_square,
               Bitboard enemey_attacks,
               bool quiet_moves);

  void generateMovesTo(Move **move_list,
                       ind square,
                       ind defender,
                       Bitboard pins,
                       Bitboard enemey_attacks);
  void evasionGen(Move **move_list,
                  Bitboard enemey_attacks,
                  Bitboard pins,
                  ind king_square);
  Bitboard attackSetGen(bool color);

  Bitboard bishopPins(ind king_square);
  Bitboard rookPins(ind king_square);

  void Serialize(Move **move_list, Bitboard b, ind source);
  void Serialize(Move **move_list, Bitboard b, ind source, ind special);
  void SerializeFromDest(Move **move_list,
                         Bitboard b,
                         ind dest,
                         ind defender,
                         ind special);

  void SerializePawn(Move **move_list, Bitboard b, ind special, int delta);
  void closeMoveList(Move **move_list);

};

}  // namespace exacto

#endif  // exacto_src_board_h
