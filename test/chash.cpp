#include "assert.h"
#include "../src/chash.cpp"

using namespace std;

// Tests that setDimensions produces appropriate number of entriesa
int testSetDimensions() {
    cout << "Testing setDimensions gives correct # of entries..." << endl;

    CHash hash(17);
    int expectedEntries = 1024 * 1024;
    ASSERT(hash.entries == expectedEntries, "setDimension gives wrong # of entries");
}

// Checks that a basic record works
int testRecordHappyPath() {
    cout << "Testing record records results..." << endl;

    CHash hash(5);
    uint64_t exampleKey = hash.entries + 1;

    hash.record(exampleKey, BOGUS_MOVE, 0, HASH_EXACT, 0);
    ASSERT(hash.probe(exampleKey, 0) == HASH_EXACT, "Probe failed after record");
}

// Checks that flag takes precedence
int testRecordFlagOrder() {
    cout << "Testing record respects flag precedence..." << endl;

    CHash hash(5);
    uint64_t exampleKey1 = hash.entries + 1;
    uint64_t exampleKey2 = 2 * hash.entries + 1;
    hash.record(exampleKey1, BOGUS_MOVE, 0, HASH_BETA, 0);
    hash.record(exampleKey2, BOGUS_MOVE, 0, HASH_ALPHA, 0);
    ASSERT(hash.probe(exampleKey2, 0) == HASH_MISS, "HASH_ALPHA overwrote HASH_BETA");
}

// Checks that record marks entry as old if not recording
int testRecordMarkFlagOld() {
    cout << "Testing record marks flag as old if not recording..." << endl;

    CHash hash(5);
    uint64_t exampleKey1 = hash.entries + 1;
    uint64_t exampleKey2 = 2 * hash.entries + 1;
    hash.record(exampleKey1, BOGUS_MOVE, 0, HASH_BETA, 0);
    hash.record(exampleKey2, BOGUS_MOVE, 0, HASH_ALPHA, 0);
    ASSERT(hash.probe(exampleKey1, 0) == HASH_BETA_OLD, "Record did not mark flag as old");
}

// Checks that record respects depth
int testRecordRespectsDepth() {
    cout << "Testing record respects depth..." << endl;

    CHash hash(5);
    uint64_t exampleKey1 = hash.entries + 1;
    uint64_t exampleKey2 = 2 * hash.entries + 1;
    hash.record(exampleKey1, BOGUS_MOVE, 3, HASH_EXACT, 0);
    ASSERT(hash.probe(exampleKey1, 3) == HASH_EXACT, "Record failed to record");
    hash.record(exampleKey2, BOGUS_MOVE, 2, HASH_EXACT, 0);
    ASSERT(hash.probe(exampleKey2, 0) == HASH_MISS, "Depth 2 overwrote depth 3");
}

int main() {
    int t = 0;

    t += testSetDimensions();
    t += testRecordHappyPath();
    t += testRecordFlagOrder();
    t += testRecordMarkFlagOld();
    t += testRecordRespectsDepth();

    cout << endl;
    cout << t << " test(s) OK" << endl;
    return 0;
}

