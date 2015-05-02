// file:     inlines.h
// created:  5/1/2015
// license:  See text/license.txt
// purpose:  Small functions which are called frequently, to be inlined.

#pragma once
#include <stdint.h>
#include "bb.h"
using namespace std;

// Intrinsic bitscans for the Microsoft compiler.
#ifdef _MSC_VER
    #if _WIN32
        #pragma intrinsic(_BitScanForward)
        #pragma intrinsic(_BitScanReverse)
    #endif
    #if _WIN64
        #pragma intrinsic(_BitScanReverse64)
        #pragma intrinsic(_BitScanForward64)
    #endif
#endif

// exp_2 is the function x /mapsto 2^x.
inline
BB exp_2(ind n) {
    return ((BB)1) << n;
}

// bitscan for the least significant set bit. Uses intrinsic bitscans for Microsoft compiler or
// builtin for GCC.
inline
#ifdef _MSC_VER
#if _WIN32
    ind bitscan(BB b) {
        unsigned long ind;
        if((uint32_t)b) {
            _BitScanForward(&ind, (uint32_t)b);
        } else {
            _BitScanForward(&ind, (uint32_t)(b >> 32));
            ind += 32;
        }
        return (ind)ind;
    }
#endif
#if _WIN64
    ind bitscan(BB b) {
        unsigned long ind;
        _BitScanForward64(&ind, b);
        return (ind)ind;
    }
#endif
#endif
#ifdef __GNUC__
    ind bitscan(BB b) {
        return __builtin_ctzll(b);
    }
#endif

// bitscanReverse finds the most significatn set bit. Uses intrinsic bitscans for Microsoft compiler
// or builtin for GCC.
inline
#ifdef _MSC_VER
#ifdef _WIN_32
    ind bitscanReverse(BB b) {
        unsigned long ind;
        if(b >> 32) {
            _BitScanReverse(&ind, b >> 32);
            ind += 32;
        } else {
            _BitScanReverse(&ind, (uint32_t)b);
        }
        return (ind)ind;
    }
#endif
#ifdef _WIN_64
    ind bitscanReverse(BB b) {
        unsigned long ind;
        _BitScanReverse64(&ind, b);
        return (ind)ind;
    }
#endif
#endif
#ifdef __GNUC__
    ind bitscanReverse(BB b) {
        return (63 - __builtin_clzll(b));
    }
#endif
