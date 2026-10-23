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

// setup a wifi client and mqtt client
WiFiSSLClient wifiClient;
PubSubClient client(wifiClient);

// definition of poll based tasks
// look at loop function
TelemetryTask telemetryTask(1.0);
BlinkTask blinkTask(LED_BUILTIN, 2.0);

// this function is called every time the device receives a message from the
// server
void callback(char *topic, byte *payload, unsigned int length) {
  // copy payload into string
  String message;
  for (unsigned int i = 0; i < length; i++)
    message += (char)payload[i];

  // print whatever we got
  // (good for debugging)
  Serial.print("Topic: ");
  Serial.println(topic);
  Serial.print("Payload: ");
  Serial.println(message);

  // turn the message into a json
  JsonDocument doc;
  if (deserializeJson(doc, message)) {
    Serial.println("JSON parse failed");
    return;
  }

  // turn the topic (char *) into a string
  String topicStr(topic);

  // here we untangle what we received.
  // did we get a request (RPC) or did an attribute change?
  //
  // this checks if the message was a request
  if (topicStr.startsWith("v1/devices/me/rpc/request/")) {
    // everything after "request/..." is the request id
    String requestId =
        topicStr.substring(String("v1/devices/me/rpc/request/").length());

    // we extract the method from the message
    const char *method = doc["method"];

    // what is the method?
    //
    // setFreq sets the frequency
    if (String(method) == "setFreq") {
      Serial.println("Received RPC setFreq.");
      // get the parameters. is it a float?
      if (doc["params"].is<float>()) {
        auto freq = doc["params"].as<float>();
        blinkTask.setFrequency(freq);
        Serial.println(String("Setting blink frequency to ") + freq);
      } else
        Serial.println("Received faulty parameter");
    }
    // getFreq returns the current frequency
    else if (String(method) == "getFreq") {
      String responseTopic = "v1/devices/me/rpc/response/" + requestId;

      float freq = blinkTask.getFrequency();
      Serial.println("Received RPC getFreq.");
      Serial.println("Responding with " + String(freq));

      // send back the response
      client.publish(responseTopic.c_str(), String(freq).c_str());
    }

  }

  // this is executed if as attribute changed
  else if (topicStr == "v1/devices/me/attributes") {
    // check if a field "blink" that holds a boolean is in the json
    if (doc["blink"].is<bool>())
      blinkTask.setEnabled(doc["blink"].as<bool>());
  }
}

void setup() {
  // setup serial connection
  Serial.begin(115200);
  while (!Serial)
    ;

  // set the led to output
  pinMode(LED_BUILTIN, OUTPUT);

  // connect to wifi
  connectWiFi(WIFI_SSID, WIFI_PASSWD);

  // set server and register callback function
  client.setServer(MQTT_ENDPOINT, MQTT_PORT);
  client.setCallback(callback);
}

void loop() {
  // each loop the connection is checked
  connectMQTT(client, CLIENT_ID, USER_NAME, PASSWD);
  // this ticks the client. it checks for incoming messages and calls the
  // callback function
  client.loop();

  // two poll based tasks to simulate parallelism
  // check out src/common/blink and src/common/telemetry
  telemetryTask.poll(client);
  blinkTask.poll();
}
