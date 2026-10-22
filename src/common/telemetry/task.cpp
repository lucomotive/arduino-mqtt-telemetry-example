

#include "task.h"
#include "Arduino.h"
#include "WiFiNINA.h"

// this is the mqtt topic the telemetry posts to.
// it is defined by thingsboard, to be accepted as telemetry by the server.
//
// other servers and mqtt broker might use other topics
const char TOPIC[] = "v1/devices/me/telemetry";

// this initializes the telemetry task.
// it only takes in a frequency, how often the device sends telemetry per second
TelemetryTask::TelemetryTask(float freq) : timer_(1000.0f / freq) {}

// like the blink task, this functions polls the TelemetryTask.
// it waits for the timer, checks if the mqtt client is connected and sends a
// simple rssi value to the server
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
