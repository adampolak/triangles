#ifndef TIMER_H
#define TIMER_H

// Walltime timer
class Timer {
 public:
  virtual ~Timer() {}

  // Returns number of milliseconds since the last call to Done, Reset, or
  // constructor and prints it with label to stderr.
  virtual int Done(const char* label) = 0;

  virtual void Reset() = 0;

  static Timer* NewTimer();
};

#endif
