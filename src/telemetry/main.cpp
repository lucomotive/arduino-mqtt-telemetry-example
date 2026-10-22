// run with
// pio run -e telemetry -t upload && pio device monitor

// expects a file like:
// const char MQTT_ENDPOINT[] = "...";
// const int MQTT_PORT = 1883;
// const char CLIENT_ID[] = "...";
// const char USER_NAME[] = "...";
// const char PASSWD[] = "...";
#include "../../env/mqtt.h"
// expects a file like:
// const char WIFI_SSID[] = "...";
// const char WIFI_PASSWD[] = "...";
#include "../../env/wifi.h"

#include "PubSubClient.h"
#include "WiFiNINA.h"
#include "WiFiSSLClient.h"
#include "common/blink/task.h"
#include "common/telemetry/task.h"
#include "timer.h"

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
  client.loop();
}

void setup() {
  Serial.begin(115200);
  while (!Serial)
    ;

  pinMode(LED_BUILTIN, OUTPUT);

  connectWiFi();
  client.setServer(MQTT_ENDPOINT, MQTT_PORT);
}

// timers for tasks (look at loop function)
Timer telemetryTimer(1000);
Timer blinkTimer(700);

void loop() {
  connectMQTT();

  // two tasks run in "parallel"
  // one sends telemetry every 1000ms and the other blinks every 700ms
  if (telemetryTimer.elapsed()) {
    telemetryTask(client);
    telemetryTimer.restart();
  }
  if (blinkTimer.elapsed()) {
    blinkTask();
    blinkTimer.restart();
  }
}
