#include "timer.h"

#include <chrono>
using namespace std;

namespace {
template<typename T>
int ToMillis(T t) {
  return chrono::duration_cast<chrono::milliseconds>(t).count();
}

class TimerImpl : public Timer {
 public:
  TimerImpl() : start(Clock::now()), last(start) {}
  virtual ~TimerImpl() {}

  virtual int SinceStart() { return ToMillis(Clock::now() - start); }
  
  virtual int SinceLast() {
    auto now = Clock::now();
    int ret = ToMillis(now - last);
    last = now;
    return ret;
    return 0;
  }

  typedef std::chrono::high_resolution_clock Clock;

 private:
  Clock::time_point start, last;
};
}  // namespace

Timer* Timer::NewTimer() { return new TimerImpl; }
