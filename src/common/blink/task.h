#ifndef BLINK_TASK_H
#define BLINK_TASK_H

#include "Arduino.h"
#include "timer.h"

// extern bool blinkState;
// void blinkTask();

class BlinkTask {
public:
  BlinkTask(pin_size_t pin, float frequency);

  void poll();
  void setEnabled(bool);

  void setFrequency(float);
  float getFrequency();

private:
  bool state_ = false;

  Timer timer_;

  pin_size_t pin_;
  bool enabled_ = true;
};

#endif
