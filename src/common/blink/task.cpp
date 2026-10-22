

#include "task.h"
#include "Arduino.h"

// bool blinkState = false;
// void blinkTask() {
//   blinkState = !blinkState;
//   digitalWrite(LED_BUILTIN, blinkState);
// }

BlinkTask::BlinkTask(pin_size_t pin, float freq)
    : timer_(1000.0f / freq / 2.0f), pin_(pin) {}

void BlinkTask::setEnabled(bool enabled) {
  this->enabled_ = enabled;
  digitalWrite(pin_, enabled ? state_ : LOW);
}
void BlinkTask::setFrequency(float freq) { timer_.set(1000.0f / freq / 2.0f); }
float BlinkTask::getFrequency() {
  return 1000.0f / (float)timer_.duration() / 2.0f;
}

void BlinkTask::poll() {
  if (!timer_.elapsed())
    return;
  timer_.restart();

  state_ = !state_;
  digitalWrite(pin_, state_ && enabled_);
}
