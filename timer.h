#ifndef TIMER_H
#define TIMER_H

#include <iostream>

// Walltime timer
class Timer {
 public:
  virtual ~Timer() {}

  virtual int SinceStart() = 0;  // ms since object construction
  virtual int SinceLast() = 0;  // ms since last call to this method or ctor

  static Timer* NewTimer();
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
