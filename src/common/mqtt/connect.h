#ifndef CONNECT_MQTT_H
#define CONNECT_MQTT_H

#include "PubSubClient.h"

void connectMQTT(PubSubClient &client, const char *const id,
                 const char *const username, const char *const passwd) {
  while (!client.connected()) {
    Serial.print("Connecting to ThingsBoard...");
    // clientId, username (token), password (none)
    if (client.connect(id, username, passwd)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.println(client.state());
      delay(2000);
    }
  }
  client.loop();
}

#endif
