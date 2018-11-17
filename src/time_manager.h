#ifndef exacto_src_time_manager_h
#define exacto_src_time_manager_h

#include <stdint.h>

namespace exacto {

class TimeManager {
 public:
  TimeManager();

  void SetLevels(int _MPS, int _base_time, int _increment);
  void GetTimeForMove(float move_number, int* ideal, int* maximum);

  int EstimateMovesLeft(int move_number);

  int MPS;
  int base_time;
  int increment;

  int time;
  int opponent_time;

  bool use_exact_time;
  uint64_t nodes_next_clock_check;
  int64_t max_clock;
};

}  // namespace exacto

#endif  // time_manager_h
