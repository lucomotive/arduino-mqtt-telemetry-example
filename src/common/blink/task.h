// this is the header of the blink file.
// is defines the class, its functions and fields.
// the functions contents are later defined in the task.cpp file
// in the same folder
#ifndef BLINK_TASK_H
#define BLINK_TASK_H

#include "Arduino.h"
#include "timer.h"

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
