#pragma once
#include "moves.h"

#define HASH_EXACT 11
#define HASH_BETA 10
#define HASH_ALPHA 9
#define HASH_EXACT_OLD 3
#define HASH_BETA_OLD 2
#define HASH_ALPHA_OLD 1
#define HASH_MISS 0
#define MARK_AS_OLD 247  // 11110111

class CHash {
 public:
  CHash(int sizeInMB = 128);
  ~CHash();

  // The transposition table: 17 bytes per entry
  // Thus, a table of size N megabytes has 61680*N entries.
  void setDimension(int sizeInMB);
  void record(uint64_t key, mv sugg, int16_t depth, uint8_t flag, int16_t val);
  uint8_t probe(uint64_t key, int16_t depth);
  void clearTable();
  // These do not check the key matches the lock
  inline int16_t getVal(uint64_t key) { return val[key % entries]; };
  inline mv getSugg(uint64_t key) { return sugg[key % entries]; };

  // Underlying data
  int entries;
  uint64_t* lock;
  mv* sugg;
  uint8_t* flag;
  int16_t* depth;
  int16_t* val;
};
