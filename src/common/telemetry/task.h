#ifndef TELEMETRY_TASK_H
#define TELEMETRY_TASK_H

#include "PubSubClient.h"
#include "timer.h"

class TelemetryTask {
public:
  explicit TelemetryTask(unsigned int duration);
  void poll(PubSubClient &);

private:
  Timer timer_;
};

#endif
