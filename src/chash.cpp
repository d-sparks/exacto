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

void CHash::record(uint64_t key, mv sugg, int16_t depth, uint8_t flag,
                   int16_t val) {
  uint64_t digest = key % entries;
  // collision resolution. Replace if either of these hold:
  // - Given flag > stored flag
  // - Given flag = stored flag && given depth >= stored depth
  // otherwise, mark the entry as old
  uint8_t storedFlag = this->flag[digest];
  if (flag > storedFlag ||
      (flag == storedFlag && depth >= this->depth[digest])) {
    this->lock[digest] = key;
    this->sugg[digest] = sugg;
    this->flag[digest] = flag;
    this->depth[digest] = depth;
    this->val[digest] = val;
  } else {
    this->flag[digest] &= MARK_AS_OLD;
  }
}

uint8_t CHash::probe(uint64_t key, int16_t depth) {
  uint64_t digest = key % entries;
  if (this->lock[digest] == key && this->depth[digest] >= depth) {
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
      depth[i] = 0;
      val[i] = 0;
    }
  }
}
