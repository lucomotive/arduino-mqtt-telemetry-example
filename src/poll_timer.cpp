
#include "poll_timer.h"
#include "Arduino.h"

Timer::Timer(unsigned int duration)
    : finish(millis() + duration), duration(duration) {}

Poll Timer::poll() {
  auto now = millis();
  if (finish > now)
    return Poll::Pending;
  finish = millis() + duration;
  return Poll::Ready;
};

void Timer::await() {
  while (this->poll())
    ;
}

void Timer::reset() { finish = millis() + duration; }
