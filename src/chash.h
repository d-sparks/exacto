#pragma once
#include "moves.h"

class CHash {
public:
    CHash(int sizeInMB = 128);
    ~CHash();
    void setDimension(int sizeInMB);

    // The transposition table: 16 bytes per entry
    // Thus, a table of size N megabytes has 65536*N entries.
    void record(uint64_t key, mv sugg, int16_t depth, int16_t val);
    bool probe(uint64_t key, int16_t depth);
    // These do not check the key matches the lock
    int16_t getVal(uint64_t key);

    // Underlying data
    int entries;
    uint64_t* lock;
    mv* sugg;
    int16_t* depth;
    int16_t* val;
};
