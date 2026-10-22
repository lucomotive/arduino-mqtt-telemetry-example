

#include "task.h"
#include "Arduino.h"

bool blinkState = false;
void blinkTask() {
  blinkState = !blinkState;
  digitalWrite(LED_BUILTIN, blinkState);
}
