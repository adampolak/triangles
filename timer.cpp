#include "timer.h"

#include <chrono>
#include <iostream>
using namespace std;

namespace {
class TimerImpl : public Timer {
 public:
  TimerImpl() : last(Clock::now()) {}
  virtual ~TimerImpl() {}

  virtual int Done(const char* label) {
    Clock::time_point now = Clock::now();
    int res = chrono::duration_cast<chrono::milliseconds>(now - last).count();
    last = now;
    cerr << label << " " << res << endl;
    return res;
  }

  virtual void Reset() {
    last = Clock::now();
  }

  typedef std::chrono::high_resolution_clock Clock;

 private:
  Clock::time_point last;
};
}  // namespace

Timer* Timer::NewTimer() { return new TimerImpl; }
