#ifndef exacto_src_hash_h
#define exacto_src_hash_h

#include "defines.h"
#include "moves.h"

namespace exacto {

class Hash {
 public:
  Hash(int size_in_mb = 128);
  ~Hash();

  // The transposition table: 17 bytes per entry
  // Thus, a table of size N megabytes has 61680*N entries.
  void set_dimension(int size_in_mb);
  void record(uint64_t key_,
              Move sugg_,
              int16_t depth_,
              uint8_t flag_,
              int16_t val_);
  uint8_t probe(uint64_t key, int16_t depth);
  void clear_table();
  // These do not check the key matches the lock
  inline int16_t get_val(uint64_t key) { return val[key % entries]; };
  inline Move get_sugg(uint64_t key) { return sugg[key % entries]; };

  // Underlying data
  int entries;
  uint64_t* lock;
  Move* sugg;
  uint8_t* flag;
  int16_t* depth;
  int16_t* val;

  // PV table
  uint64_t* pv_lock;
  Move* pv_move;
  #define PV_SIZE 32768
  inline Move get_pv(uint64_t key) { return pv_move[key % PV_SIZE]; }

};

}  // namespace exacto

#endif  // exacto_src_hash_h
