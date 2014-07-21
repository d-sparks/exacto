// includes.h
// created: 11:43 AM 2/2/2014
// author: Daniel Sparks
// license: See text/license.txt
// purpose: All C++ includes used in exacto.

#include <iostream>
#include <iomanip>
#include <fstream>
#include <math.h>
#include <string>

#ifdef _MSC_VER
    #include <stdint.h>
    #include <minmax.h>
    #include <time.h>
    #include <intrin.h>
#endif

#ifdef __GNUC__
    #include <stdlib.h>
#endif

using namespace std;