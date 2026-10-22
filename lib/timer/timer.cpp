
#include "timer.h"
#include "Arduino.h"

Timer::Timer(unsigned int duration) : start_(millis()), duration_(duration) {}

bool Timer::elapsed() {
  auto now = millis();
  auto finish = this->start_ + this->duration_;
  return finish <= now;
};

void Timer::restart() { start_ = millis(); }
void Timer::set(unsigned int duration) { this->duration_ = duration; }
unsigned int Timer::duration() { return duration_; }
