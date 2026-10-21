// run with
// pio run -t upload && pio device monitor

// expects a file like:
// const char MQTT_ENDPOINT[] = "...";
// const int MQTT_PORT = 1883;
// const char CLIENT_ID[] = "...";
// const char USER_NAME[] = "...";
// const char PASSWD[] = "...";
// const char TOPIC[] = "v1/devices/me/telemetry";
#include "../env/mqtt.h"
// expects a file like:
// const char WIFI_SSID[] = "...";
// const char WIFI_PASSWD[] = "...";
#include "../env/wifi.h"

#include "PubSubClient.h"
#include "WiFiNINA.h"
#include "WiFiSSLClient.h"
#include "api/Common.h"
#include "poll_timer.h"
#include "variant.h"

WiFiSSLClient wifiClient;
PubSubClient client(wifiClient);

void connectWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("\n");
  Serial.println("WiFi connected");
}
void connectMQTT() {
  while (!client.connected()) {
    Serial.print("Connecting to ThingsBoard...");
    // clientId, username (token), password (none)
    if (client.connect(CLIENT_ID, USER_NAME, PASSWD)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.println(client.state());
      delay(2000);
    }
  }
}
void mqttTask() {
  if (!client.connected())
    connectMQTT();
  client.loop();

  int rssi = WiFi.RSSI();

  // ThingsBoard expects JSON on this topic
  char payload[32];
  snprintf(payload, sizeof(payload), "{\"rssi\": %d}", rssi);

  client.publish(TOPIC, payload);
  Serial.print("Sent: ");
  Serial.println(payload);
}

bool blinkState = false;
void blinkTask() {
  blinkState = !blinkState;
  digitalWrite(LED_BUILTIN, blinkState);
}

void setup() {
  Serial.begin(115200);
  while (!Serial)
    ;

  pinMode(LED_BUILTIN, OUTPUT);

  connectWiFi();
  client.setServer(MQTT_ENDPOINT, MQTT_PORT);
}

Timer mqttTimer(1000);
Timer blinkTimer(700);

void loop() {
  if (mqttTimer.poll())
    mqttTask();
  if (blinkTimer.poll())
    blinkTask();
}
