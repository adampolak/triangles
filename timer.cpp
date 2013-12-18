#include "timer.h"

using namespace std;

namespace {
template<typename T>
int ToMillis(T t) {
  return chrono::duration_cast<chrono::milliseconds>(t).count();
}
}  // namespace

Timer::Timer() : start(Clock::now()), last(start) {
}

int Timer::SinceStart() { return ToMillis(Clock::now() - start); }

int Timer::SinceLast() {
  auto now = Clock::now();
  int ret = ToMillis(now - last);
  last = now;
  return ret;
}
