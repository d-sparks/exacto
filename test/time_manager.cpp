#include "../src/time_manager.h"

#include <iostream>
#include <string>

#include "assert.h"

using namespace exacto;
using namespace std;

// Tests that moves are sorted appropriately
int TestBasicTimeControl() {
  cout << "Testing basic time control..." << endl;

  TimeManager time_manager;

  int ideal;
  int maximal;
  time_manager.GetTimeForMove(0, 40, 40 * 60 * 100, &ideal, &maximal);

  ASSERT_EQ(ideal, 120 * 100, "Ideal time wrong: " + to_string(ideal));
  ASSERT_EQ(maximal, 180 * 100, "Maximal time wrong: " + to_string(maximal));

  return 1;
}

int main() {
  int t = 0;

  t += TestBasicTimeControl();

  cout << endl;
  cout << t << " test(s) OK" << endl;
  return 0;
}
