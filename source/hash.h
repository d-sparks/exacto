// hash.h
// created: 5:09PM 2/26/14
// author: Daniel Sparks
// license: See text/license.txt
// purpose: Header file for the hash table class.

#pragma once

#include "includes.h"

class CHash {
public:

    void                set_dimension               (unsigned int size_in_mb);
    void                clear_table                 ();
    void                record                      (uint64_t key, uint16_t suggestion, uint8_t depth, uint8_t flag, int16_t score);
    uint8_t             probe                       (uint64_t key, uint8_t depth);
    void                delete_entry                (uint64_t key);
    bool                clear_repititions           (CGame * game, uint8_t depth);

    uint16_t            * old_depth_flag;           // ========= The main transposition table ==========
    uint16_t            * sugg;                     // 14 bytes per entry, variable number of entries
    int16_t             * value;                    // depth_flag has 2 bits storing hash flag, and 6 bits storing depth
    uint64_t            * lock;
    unsigned int        size;

    uint64_t            * PV_lock;                  // ========= A hash table just for the PV ==========
    uint16_t            * PV_move;                  // v entries, 10 bytes per entry = ?
    #define             PV_size                     32768


    CHash(void);
    ~CHash(void);

};