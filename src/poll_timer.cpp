#pragma once

#include "Arduino.h"
#include "api/Common.h"

enum Poll { Pending = 0, Ready = 1 };

class Timer {
private:
  unsigned long finish;
  unsigned int duration;

public:
  Timer(unsigned int duration)
      : finish(millis() + duration), duration(duration) {}

  Poll poll() {
    auto now = millis();
    if (finish > now)
      return Pending;
    finish = millis() + duration;
    return Ready;
  };
  void await() {
    while (this->poll())
      ;
  }

  void reset() { finish = millis() + duration; }
};
