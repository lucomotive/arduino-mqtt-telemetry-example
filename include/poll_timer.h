#ifndef POLL_TIMER_H
#define POLL_TIMER_H

enum Poll { Pending = 0, Ready = 1 };

class Timer {

public:
  Timer(unsigned int duration);

  Poll poll();
  void await();
  void reset();

private:
  unsigned long finish;
  unsigned int duration;
};

#endif
