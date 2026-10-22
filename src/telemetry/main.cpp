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
#include "common/mqtt/connect.h"
#include "common/telemetry/task.h"
#include "common/wifi/connect.h"

// setup a wifi client and mqtt client
WiFiSSLClient wifiClient;
PubSubClient client(wifiClient);

void setup() {
  Serial.begin(115200);
  while (!Serial)
    ;

  pinMode(LED_BUILTIN, OUTPUT);

  connectWiFi(WIFI_SSID, WIFI_PASSWD);
  client.setServer(MQTT_ENDPOINT, MQTT_PORT);
}

// definition of poll based tasks
// look at loop function
TelemetryTask telemetryTask(1.0);
BlinkTask blinkTask(LED_BUILTIN, 2.0);

void loop() {
  connectMQTT(client, CLIENT_ID, USER_NAME, PASSWD);
  client.loop();

  // two tasks run in "parallel"
  // one sends telemetry every 1000ms and the other blinks every 700ms
  telemetryTask.poll(client);
  blinkTask.poll();
}
