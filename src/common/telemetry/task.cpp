

#include "task.h"
#include "../../env/mqtt.h"
#include "Arduino.h"
#include "WiFiNINA.h"

const char TOPIC[] = "v1/devices/me/telemetry";

void telemetryTask(PubSubClient &client) {
  // only works if mqtt is connected
  if (!client.connected())
    return;

  int rssi = WiFi.RSSI();

  // ThingsBoard expects JSON on this topic
  char payload[32];
  snprintf(payload, sizeof(payload), "{\"rssi\": %d}", rssi);

  client.publish(TOPIC, payload);
  Serial.print("Sent: ");
  Serial.println(payload);
}
