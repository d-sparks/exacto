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
        unsigned long i;
        if((uint32_t)b) {
            _BitScanForward(&i, (uint32_t)b);
        } else {
            _BitScanForward(&i, (uint32_t)(b >> 32));
            i += 32;
        }
        return (ind)i;
    }
#endif
#if _WIN64
    ind bitscan(BB b) {
        unsigned long i;
        _BitScanForward64(&i, b);
        return (ind)i;
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
        unsigned long i;
        if(b >> 32) {
            _BitScanReverse(&i, b >> 32);
            i += 32;
        } else {
            _BitScanReverse(&i, (uint32_t)b);
        }
        return (ind)i;
    }
#endif
#ifdef _WIN_64
    ind bitscanReverse(BB b) {
        unsigned long i;
        _BitScanReverse64(&i, b);
        return (ind)i;
    }
#endif
#endif
#ifdef __GNUC__
    ind bitscanReverse(BB b) {
        return (63 - __builtin_clzll(b));
    }
#endif
