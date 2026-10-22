#ifndef TELEMETRY_TASK_H
#define TELEMETRY_TASK_H

#include "PubSubClient.h"
#include "timer.h"

class TelemetryTask {
public:
  explicit TelemetryTask(float freq);
  void poll(PubSubClient &);

private:
  Timer timer_;
};

#endif
