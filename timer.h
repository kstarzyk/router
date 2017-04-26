#ifndef _TIMER_H_
#define _TIMER_H_ 1

#include <chrono>
using namespace std::chrono;

class Timer {
  public:
  steady_clock::time_point static now() {
    return steady_clock::now();
  }

  int static wait(steady_clock::time_point &start, int targetTime) {
    steady_clock::time_point end = steady_clock::now();
    typedef duration<int,std::milli> millisecs_t;
    millisecs_t duration(duration_cast<millisecs_t>(end-start));
    return ((int) duration.count() < targetTime);
  }

};

#endif
