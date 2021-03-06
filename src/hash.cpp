#include "hash.h"

#include <cstddef>

namespace exacto {

Hash::Hash(int size_in_mb) {
  lock = NULL;
  sugg = NULL;
  flag = NULL;
  depth = NULL;
  val = NULL;
  entries = 0;
  pv_lock = NULL;
  pv_move = NULL;
  set_dimension(size_in_mb);
}

Hash::~Hash() { set_dimension(0); }

void Hash::set_dimension(int size_in_mb) {
  if (size_in_mb > 0) {
    // Number of entries is size_in_mb * 1024^2 / (8 + 4 + 1 + 2 + 2)
    // or 61680 * size_in_mb;
    entries = 1024 * 1024 * size_in_mb / 17;
  }
  clear_table();
}

void Hash::record(uint64_t key_, Move sugg_, int16_t depth_, uint8_t flag_,
                  int16_t val_) {
  uint64_t digest = key_ % entries;
  // collision resolution. Replace if either of these hold:
  // - Given flag > stored flag
  // - Given flag = stored flag && given depth >= stored depth
  // otherwise, mark the entry as old
  uint8_t stored_flag = flag[digest];
  if ((flag_ > stored_flag) ||
      (flag_ == stored_flag && depth_ >= depth[digest])) {
    lock[digest] = key_;
    sugg[digest] = sugg_;
    flag[digest] = flag_;
    depth[digest] = depth_;
    val[digest] = val_;
  } else {
    flag[digest] &= MARK_AS_OLD;
  }
  if (flag_ == HASH_EXACT) {
    pv_lock[key_ % PV_SIZE] = key_;
    pv_move[key_ % PV_SIZE] = sugg_;
  }
}

uint8_t Hash::probe(uint64_t key_, int16_t depth_) {
  uint64_t digest = key_ % entries;
  if (lock[digest] == key_ && depth[digest] >= depth_) {
    return flag[digest];
  }
  return HASH_MISS;
}

void Hash::clear_table() {
  delete lock;
  delete sugg;
  delete depth;
  delete flag;
  delete val;
  if (entries > 0) {
    lock = new uint64_t[entries];
    sugg = new Move[entries];
    depth = new int16_t[entries];
    flag = new uint8_t[entries];
    val = new int16_t[entries];
    for (int i = 0; i < entries; ++i) {
      lock[i] = 0;
      sugg[i] = 0;
      flag[i] = 0;
      depth[i] = -1;
      val[i] = 0;
    }
    delete pv_lock;
    delete pv_move;
    pv_lock = new uint64_t[PV_SIZE];
    pv_move = new Move[PV_SIZE];
    for (int i = 0; i < PV_SIZE; ++i) {
      pv_lock[i] = 0;
      pv_move[i] = 0;
    }
  }
}

}  // namespace exacto
