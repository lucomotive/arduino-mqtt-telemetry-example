

#include "task.h"
#include "Arduino.h"

// this constructor takes a pin and a frequency.
// the pin specifies the pin the blinking led is on.
// the frequency specifies, how fast the led should blink.
//
// the frequency is converted to ms and sets the duration of the timer
BlinkTask::BlinkTask(pin_size_t pin, float freq)
    : timer_(1000.0f / freq / 2.0f), pin_(pin) {}

// this enables/disables the task
void BlinkTask::setEnabled(bool enabled) {
  this->enabled_ = enabled;
  digitalWrite(pin_, enabled ? state_ : LOW);
}

// this functions sets the frequency, to easily change it in the main.cpp file
void BlinkTask::setFrequency(float freq) { timer_.set(1000.0f / freq / 2.0f); }
float BlinkTask::getFrequency() {
  return 1000.0f / (float)timer_.duration() / 2.0f;
}

// this function polls the task
void BlinkTask::poll() {
  // the timer is checked first thing on every poll
  // if it isnt elapsed, the poll returns and the task does nothing
  //   -> the task waits for the timer
  if (!timer_.elapsed())
    return;
  // if the timer is elapsed, its reset, so it can run again
  timer_.restart();

  // the state is reversed
  // on -> off
  // off -> on
  state_ = !state_;
  // the led is set according to the state,
  // but it is only on, if both the state and enabled variable are true
  digitalWrite(pin_, state_ && enabled_);
}
