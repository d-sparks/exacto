#pragma once
#include "chash.h"

CHash::CHash(int sizeInMB) {
    lock = NULL;
    sugg = NULL;
    depth = NULL;
    val = NULL;
    setDimension(sizeInMB);
}

CHash::~CHash() {
    setDimension(0);
}

void CHash::setDimension(int sizeInMB) {
    delete lock;
    delete sugg;
    delete depth;
    delete val;
    if(sizeInMB > 0) {
        // Number of entries is sizeInMB * 1024^2 / (8 + 4 + 2 + 2)
        // or 65536 * sizeInMB;
        entries = 1024 * 1024 * sizeInMB / 16;
        lock = new uint64_t[entries];
        sugg = new mv[entries];
        depth = new int16_t[entries];
        val = new int16_t[entries];
        for(int i = 0; i < entries; i++) {
            lock[i] = 0;
            sugg[i] = 0;
            depth[i] = 0;
            val[i] = 0;
        }
    }
}

void CHash::record(uint64_t key, mv sugg, int16_t depth, int16_t val) {
    uint64_t digest = key % entries;
    this->lock[digest] = key;
    this->sugg[digest] = sugg;
    this->depth[digest] = depth;
    this->val[digest] = val;
}

bool CHash::probe(uint64_t key, int16_t depth) {
    uint64_t digest = key % entries;
    return this->lock[digest] == key && this->depth[digest] >= depth;
}

int16_t CHash::getVal(uint64_t key) {
    return val[key % entries];
}
