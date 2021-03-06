#include "../src/hash.h"

#include "assert.h"

using namespace exacto;
using namespace std;

// Tests that set_dimensions produces appropriate number of entriesa
int TestSetDimensions() {
  cout << "Testing set_dimensions gives correct # of entries..." << endl;

  Hash hash(17);
  int expectedEntries = 1024 * 1024;
  ASSERT(hash.entries == expectedEntries,
         "set_dimension gives wrong # of entries");

  return 1;
}

// Checks that a basic record works
int TestRecordHappyPath() {
  cout << "Testing record records results..." << endl;

  Hash hash(5);
  uint64_t exampleKey = hash.entries + 1;

  hash.record(exampleKey, BOGUS_MOVE, 0, HASH_EXACT, 0);
  ASSERT(hash.probe(exampleKey, 0) == HASH_EXACT, "Probe failed after record");

  return 1;
}

// Checks that flag takes precedence
int TestRecordFlagOrder() {
  cout << "Testing record respects flag precedence..." << endl;

  Hash hash(5);
  uint64_t exampleKey1 = hash.entries + 1;
  uint64_t exampleKey2 = 2 * hash.entries + 1;
  hash.record(exampleKey1, BOGUS_MOVE, 0, HASH_BETA, 0);
  hash.record(exampleKey2, BOGUS_MOVE, 0, HASH_ALPHA, 0);
  ASSERT(hash.probe(exampleKey2, 0) == HASH_MISS,
         "HASH_ALPHA overwrote HASH_BETA");
  
  return 1;
}

// Checks that record marks entry as old if not recording
int TestRecordMarkFlagOld() {
  cout << "Testing record marks flag as old if not recording..." << endl;

  Hash hash(5);
  uint64_t exampleKey1 = hash.entries + 1;
  uint64_t exampleKey2 = 2 * hash.entries + 1;
  hash.record(exampleKey1, BOGUS_MOVE, 0, HASH_BETA, 0);
  hash.record(exampleKey2, BOGUS_MOVE, 0, HASH_ALPHA, 0);
  ASSERT(hash.probe(exampleKey1, 0) == HASH_BETA_OLD,
         "Record did not mark flag as old");

  return 1;
}

// Checks that record respects depth
int TestRecordRespectsDepth() {
  cout << "Testing record respects depth..." << endl;

  Hash hash(5);
  uint64_t exampleKey1 = hash.entries + 1;
  uint64_t exampleKey2 = 2 * hash.entries + 1;
  hash.record(exampleKey1, BOGUS_MOVE, 3, HASH_EXACT, 0);
  ASSERT(hash.probe(exampleKey1, 3) == HASH_EXACT, "Record failed to record");
  hash.record(exampleKey2, BOGUS_MOVE, 2, HASH_EXACT, 0);
  ASSERT(hash.probe(exampleKey2, 0) == HASH_MISS, "Depth 2 overwrote depth 3");

  return 1;
}

int main() {
  int t = 0;

  t += TestSetDimensions();
  t += TestRecordHappyPath();
  t += TestRecordFlagOrder();
  t += TestRecordMarkFlagOld();
  t += TestRecordRespectsDepth();

  cout << endl;
  cout << t << " test(s) OK" << endl;
  return 0;
}
