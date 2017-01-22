#pragma once
#include "chash.h"
#include <cstddef>

CHash::CHash(int sizeInMB) {
  lock = NULL;
  sugg = NULL;
  flag = NULL;
  depth = NULL;
  val = NULL;
  entries = 0;
  PV_lock = NULL;
  PV_move = NULL;
  setDimension(sizeInMB);
}

CHash::~CHash() { setDimension(0); }

void CHash::setDimension(int sizeInMB) {
  if (sizeInMB > 0) {
    // Number of entries is sizeInMB * 1024^2 / (8 + 4 + 1 + 2 + 2)
    // or 61680 * sizeInMB;
    entries = 1024 * 1024 * sizeInMB / 17;
  }
  clearTable();
}

void CHash::record(uint64_t key_, mv sugg_, int16_t depth_, uint8_t flag_,
                   int16_t val_) {
  uint64_t digest = key_ % entries;
  // collision resolution. Replace if either of these hold:
  // - Given flag > stored flag
  // - Given flag = stored flag && given depth >= stored depth
  // otherwise, mark the entry as old
  uint8_t storedFlag = flag[digest];
  if ((flag_ > storedFlag) ||
      (flag_ == storedFlag && depth_ >= depth[digest])) {
    lock[digest] = key_;
    sugg[digest] = sugg_;
    flag[digest] = flag_;
    depth[digest] = depth_;
    val[digest] = val_;
  } else {
    flag[digest] &= MARK_AS_OLD;
  }
  if (flag_ == HASH_EXACT) {
    PV_lock[key_ % PV_SIZE] = key_;
    PV_move[key_ % PV_SIZE] = sugg_;
  }
}

uint8_t CHash::probe(uint64_t key_, int16_t depth_) {
  uint64_t digest = key_ % entries;
  if (lock[digest] == key_ && depth[digest] >= depth_) {
    return flag[digest];
  }
  return HASH_MISS;
}

void CHash::clearTable() {
  delete lock;
  delete sugg;
  delete depth;
  delete flag;
  delete val;
  if (entries > 0) {
    lock = new uint64_t[entries];
    sugg = new mv[entries];
    depth = new int16_t[entries];
    flag = new uint8_t[entries];
    val = new int16_t[entries];
    for (int i = 0; i < entries; i++) {
      lock[i] = 0;
      sugg[i] = 0;
      flag[i] = 0;
      depth[i] = -1;
      val[i] = 0;
    }
    delete PV_lock;
    delete PV_move;
    PV_lock = new uint64_t[PV_SIZE];
    PV_move = new mv[PV_SIZE];
    for (int i = 0; i < PV_SIZE; i++) {
      PV_lock[i] = 0;
      PV_move[i] = 0;
    }
  }
}
