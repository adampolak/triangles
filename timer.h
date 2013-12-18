#ifndef TIMER_H
#define TIMER_H

#include <chrono>
#include <iostream>

// Walltime timer
class Timer {
 public:
  Timer();
  ~Timer() {}

  int SinceStart();  // ms since object construction
  int SinceLast();  // ms since last call to this method or object construction

  typedef std::chrono::high_resolution_clock Clock;

 private:
  Clock::time_point start, last;
};

// Convenient wrapper for writing to stderr. Automagically appends end-of-line.
// Example usage:
//   Log() << "Some log message";
struct Log {
  ~Log() { std::cerr << std::endl; }
};

template<typename T>
const Log& operator<<(const Log& log, const T& t) {
  std::cerr << t;
  return log;
}


#endif
