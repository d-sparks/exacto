#pragma once
#include <iostream>
using namespace std;

#define ASSERT(condition, message) \
do { \
    if (! (condition)) { \
        cerr << "Assertion `" #condition "` failed in " << __FILE__ \
            << " line " << __LINE__ << ": " << message << endl; \
        exit(EXIT_FAILURE); \
    } \
} while (false)
