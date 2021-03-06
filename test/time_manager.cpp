#define _TEST 1

#include "../src/time_manager.h"

#include <iostream>
#include <string>

#include "assert.h"

using namespace exacto;
using namespace std;

// Happy path.
int TestBasicTimeControl() {
  cout << "Testing basic time control..." << endl;

  TimeManager time_manager;
  time_manager.SetLevels(40, -1, 0);
  time_manager.time = 40 * 60 * 100;

  int ideal;
  int maximal;

  time_manager.GetTimeForMove(1, &ideal, &maximal);
  ASSERT_EQ(ideal, 114 * 100, "Ideal time wrong: " + to_string(ideal));
  ASSERT_EQ(maximal, 180 * 100, "Maximal time wrong: " + to_string(maximal));

  return 1;
}

// Test with increments.
int TestWithIncrement() {
  cout << "Testing incremental control..." << endl;

  TimeManager time_manager;
  time_manager.SetLevels(40, -1, 500);
  time_manager.time = 40 * 60 * 100;

  int ideal;
  int maximal;

  time_manager.GetTimeForMove(1, &ideal, &maximal);
  ASSERT_EQ(ideal, 119 * 100, "Ideal time wrong: " + to_string(ideal));
  ASSERT_EQ(maximal, 180 * 100, "Maximal time wrong: " + to_string(maximal));

  return 1;
}

int TestEstimateMovesLeft() {
  cout << "Testing EstimateMovesLeft..." << endl;

  TimeManager time_manager;

  time_manager.SetLevels(40, 40 * 60 * 100, 0);

  ASSERT_EQ(time_manager.EstimateMovesLeft(1), 40, "Expected 40 moves left.");
  ASSERT_EQ(time_manager.EstimateMovesLeft(40), 1, "Expected 1 moves left.");
  ASSERT_EQ(time_manager.EstimateMovesLeft(41), 40, "Expected 40 moves left.");
  ASSERT_EQ(time_manager.EstimateMovesLeft(80), 1, "Expected 1 moves left.");

  return 1;
}

int main() {
  int t = 0;

  t += TestBasicTimeControl();
  t += TestWithIncrement();
  t += TestEstimateMovesLeft();

  cout << endl;
  cout << t << " test(s) OK" << endl;
  return 0;
}
