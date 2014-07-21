// inlines.h
// created: 1:07 AM, 2/24/2014
// author: Daniel Sparks
// license: See text/license.txt
// purpose: Small functions which are called frequently, to be inlined.

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

inline
uint8_t bitscan(uint64_t bb) {

    #ifdef __GNUC__
        return __builtin_ctzll(bb);
    #endif
    #ifdef _MSC_VER
        unsigned long index;
        #if _WIN64
            _BitScanForward64(&index, bb);
        #endif
        #if _WIN32
            if((uint32_t)bb) _BitScanForward(&index, (uint32_t)bb);
            else {
                _BitScanForward(&index, (uint32_t)(bb >> 32));
                index += 32;
            }
        #endif
        return (uint8_t)index;
    #endif

}

inline
uint8_t bitscan_reverse(uint64_t bb) {

    #ifdef __GNUC__
        return (63 - __builtin_clzll(bb));
    #endif
    #ifdef _MSC_VER
        unsigned long index;
        #if _WIN64
            _BitScanReverse64(&index, bb);
        #endif
        #if _WIN32
            if(bb >> 32) {
                _BitScanReverse(&index, bb >> 32);
                index += 32;
            } else _BitScanReverse(&index, (uint32_t)bb);
        #endif
        return (uint8_t)index;
    #endif

}

inline
char pop_count(uint64_t bb) {

    #ifdef __GNUC__
        return __builtin_popcountll(bb);
    #endif
    #ifdef _MSC_VER
        uint8_t i = 0;
        while(bb) {
            i++;
            bb &= bb - 1;
        }
        return i;
    #endif

}

inline
bool pop_geq_2(uint64_t bb) {
    if((bb != 0) && ((bb & (bb - 1)) != 0)) return true;
    return false;
}

inline
bool pop_leq_1(uint64_t bb) {
    if((bb & (bb - 1)) == 0) return true;
    return false;
}

inline
uint64_t exp_2(uint8_t n) {
    return ((uint64_t)1) << n;
}

/*  I got the idea for these two functions from Andrew Backes, author of DirtyBit.  */

inline
uint64_t maskfile(uint8_t i) {
    return ((uint64_t)0x0101010101010101) << i;
}

inline
uint64_t maskrank(uint8_t i) {
    return ((uint64_t)0x00000000000000ff) << (8 * i);
}