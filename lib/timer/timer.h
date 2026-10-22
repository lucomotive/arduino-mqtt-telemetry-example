#ifndef POLL_TIMER_H
#define POLL_TIMER_H

class Timer {
public:
  Timer(unsigned int);

  bool elapsed();
  void restart();
  void set(unsigned int);
  unsigned int duration();

private:
  unsigned long start_;
  unsigned int duration_;
};

#endif
