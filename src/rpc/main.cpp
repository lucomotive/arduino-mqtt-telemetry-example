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
      client.subscribe("v1/devices/me/rpc/request/+");
      client.subscribe("v1/devices/me/attributes");
    } else {
      Serial.print("failed, rc=");
      Serial.println(client.state());
      delay(2000);
    }
  }
  client.loop();
}

// states
bool blinkEnabled = true;

// timers for tasks (look at loop function)
Timer telemetryTimer(1000);
Timer blinkTimer(500);

void callback(char *topic, byte *payload, unsigned int length) {
  // Copy payload into a null-terminated string
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
        float freq = doc["params"].as<float>();
        blinkTimer.set((unsigned int)(1000.0f / freq));

        Serial.println(String("Setting blink frequency to ") + freq);
      } else
        Serial.println("Received faulty parameter");
    } else if (String(method) == "getFreq") {
      String responseTopic = "v1/devices/me/rpc/response/" + requestId;
      float freq = 1000.0f / (float)blinkTimer.duration();
      String response = String("{\"value\":") + freq + "}";

      Serial.println("Received RPC getFreq.");
      Serial.println("Responding with " + response);

      client.publish(responseTopic.c_str(), response.c_str());
    }

  }

  // ---- Shared attribute update ----
  else if (topicStr == "v1/devices/me/attributes") {
    if (doc["blink"].is<bool>())
      blinkEnabled = doc["blink"].as<bool>();
  }
}

void setup() {
  Serial.begin(115200);
  while (!Serial)
    ;

  pinMode(LED_BUILTIN, OUTPUT);

  connectWiFi();
  client.setServer(MQTT_ENDPOINT, MQTT_PORT);
  client.setCallback(callback);
}

void loop() {
  connectMQTT();

  // two tasks run in "parallel"
  if (telemetryTimer.elapsed()) {
    telemetryTask(client);
    telemetryTimer.restart();
  }

  if (blinkTimer.elapsed()) {
    blinkState = !(blinkState && blinkEnabled);
    blinkTask();
    blinkTimer.restart();
  }
}
