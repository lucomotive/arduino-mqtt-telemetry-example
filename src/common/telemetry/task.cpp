

#include "task.h"
#include "Arduino.h"
#include "WiFiNINA.h"

const char TOPIC[] = "v1/devices/me/telemetry";

TelemetryTask::TelemetryTask(unsigned int duration) : timer_(duration) {}

void TelemetryTask::poll(PubSubClient &client) {
  if (!timer_.elapsed())
    return;
  timer_.restart();

  // only works if mqtt is connected
  if (!client.connected()) {
    Serial.println("Client not connected. Can not send telemetry.");
    return;
  }

  int rssi = WiFi.RSSI();

  // thingsBoard expects JSON on this topic
  char payload[32];
  snprintf(payload, sizeof(payload), "{\"rssi\": %d}", rssi);
  client.publish(TOPIC, payload);

  Serial.print("Telemetry sent: ");
  Serial.println(payload);
}
