#include "time_manager.h"

#include <algorithm>
#include <cmath>

namespace exacto {

void TimeManager::SetLevels(int _MPS, int _base_time, int _increment) {
  MPS = _MPS;
  base_time = _base_time;
  increment = _increment;
}

void TimeManager::GetTimeForMove(float move_number,
                                 float time_left,
                                 int* ideal,
                                 int* maximum) {

  // If less than 500 centiseconds left,
  if (time_left < 100) {
    *ideal = 0;
    *maximum = *ideal;
    return;
  }

  int moves_left = EstimateMovesLeft(move_number);

  if (moves_left < 4) {
    *ideal = 0.9f * (time_left / moves_left);
    *maximum = *ideal;
    return;
  }

  // From Robert Hyatt's Using Time Wisely.
  move_number = std::min(move_number, 10.0f);
  float ideal_factor = 2 - move_number / 10.0f;
  float max_factor = 3;
  float time = time_left / moves_left;
  *ideal = round(ideal_factor * time);
  *maximum = round(max_factor * time);
}

int TimeManager::EstimateMovesLeft(int move_number) {
  if (MPS != 0) {
    return (MPS - move_number % MPS) % MPS + 1;
  }

  return std::max(42 - move_number, 15);
}

}  // namespace exacto
