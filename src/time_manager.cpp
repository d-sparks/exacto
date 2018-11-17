#include "time_manager.h"

#include <algorithm>
#include <cmath>

namespace exacto {

TimeManager::TimeManager() {
  SetLevels(40, 5 * 60 * 100, 0);
  use_exact_time = false;
}

void TimeManager::SetLevels(int _MPS, int _base_time, int _increment) {
  MPS = _MPS;
  base_time = _base_time;
  increment = _increment;

  time = base_time;
  opponent_time = base_time;
}

void TimeManager::GetTimeForMove(float move_number, int* ideal, int* maximum) {
  // If less than 100 centiseconds left,
  if (time < 100) {
    *ideal = 0;
    *maximum = *ideal;
    return;
  }

  // Exact timing with 10 centisecond buffer
  if (use_exact_time) {
    *ideal = time - 10;
    *maximum = time - 10;
    return;
  }

  // Otherwise, actual time management
  int moves_left = EstimateMovesLeft(move_number);

  if (moves_left < 4) {
    *ideal = 0.9f * (time / moves_left);
    *maximum = *ideal;
    return;
  }

  // From Robert Hyatt's Using Time Wisely.
  move_number = std::min(move_number, 10.0f);
  float ideal_factor = 2 - move_number / 10.0f;
  float target_time = time / moves_left;
  *maximum = round(3 * target_time);
  *ideal = round(ideal_factor * target_time) + increment;
  *ideal = std::min(*ideal, *maximum);
}

int TimeManager::EstimateMovesLeft(int move_number) {
  if (MPS != 0) {
    return (MPS - move_number % MPS) % MPS + 1;
  }

  return std::max(42 - move_number, 15);
}

}  // namespace exacto
