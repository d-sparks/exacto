#ifndef exacto_src_time_manager_h
#define exacto_src_time_manager_h

namespace exacto {

class TimeManager {
 public:
  void SetLevels(int _MPS, int _base_time, int _increment);
  void GetTimeForMove(float move_number, int* ideal, int* maximum);

  void set_time(int _time) { time = _time; }
  void set_opponent_time(int otime) { opponent_time = otime; }

#ifndef _TEST
 private:
#endif
  int EstimateMovesLeft(int move_number);

  int MPS;
  int base_time;
  int increment;

  int time;
  int opponent_time;
};

}  // namespace exacto

#endif  // time_manager_h
