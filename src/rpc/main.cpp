// run with
// pio run -e rpc -t upload && pio device monitor

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

#include "ArduinoJson.h"
#include "PubSubClient.h"
#include "WiFiNINA.h"
#include "WiFiSSLClient.h"
#include "common/blink/task.h"
#include "common/mqtt/connect.h"
#include "common/telemetry/task.h"
#include "common/wifi/connect.h"

WiFiSSLClient wifiClient;
PubSubClient client(wifiClient);

// definition of poll based tasks
// look at loop function
TelemetryTask telemetryTask(1000);
BlinkTask blinkTask(LED_BUILTIN, 2.0);

void callback(char *topic, byte *payload, unsigned int length) {
  // copy payload into string
  String message;
  for (unsigned int i = 0; i < length; i++)
    message += (char)payload[i];

  Serial.print("Topic: ");
  Serial.println(topic);
  Serial.print("Payload: ");
  Serial.println(message);

  JsonDocument doc;
  if (deserializeJson(doc, message)) {
    Serial.println("JSON parse failed");
    return;
  }

  String topicStr(topic);

  // ---- RPC request ----
  if (topicStr.startsWith("v1/devices/me/rpc/request/")) {
    String requestId =
        topicStr.substring(String("v1/devices/me/rpc/request/").length());
    const char *method = doc["method"];

    if (String(method) == "setFreq") {
      Serial.println("Received RPC setFreq.");
      if (doc["params"].is<float>()) {
        auto freq = doc["params"].as<float>();
        blinkTask.setFrequency(freq);
        Serial.println(String("Setting blink frequency to ") + freq);
      } else
        Serial.println("Received faulty parameter");
    } else if (String(method) == "getFreq") {
      String responseTopic = "v1/devices/me/rpc/response/" + requestId;
      String response = String("{\"value\":") + blinkTask.getFrequency() + "}";

      Serial.println("Received RPC getFreq.");
      Serial.println("Responding with " + response);

      client.publish(responseTopic.c_str(), response.c_str());
    }

  }

  // ---- Shared attribute update ----
  else if (topicStr == "v1/devices/me/attributes") {
    if (doc["blink"].is<bool>())
      blinkTask.setEnabled(doc["blink"].as<bool>());
  }
}

void setup() {
  Serial.begin(115200);
  while (!Serial)
    ;

  pinMode(LED_BUILTIN, OUTPUT);

  connectWiFi(WIFI_SSID, WIFI_PASSWD);

  client.setServer(MQTT_ENDPOINT, MQTT_PORT);
  client.setCallback(callback);
}

void loop() {
  connectMQTT(client, CLIENT_ID, USER_NAME, PASSWD);

  // two poll based tasks
  // simulate parallelism
  telemetryTask.poll(client);
  blinkTask.poll();
}
