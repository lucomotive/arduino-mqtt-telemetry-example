#ifndef CONNECT_WIFI_H
#define CONNECT_WIFI_H

#include "Arduino.h"
#include "WiFiNINA.h"

// this function connects the device to a wifi network
void connectWiFi(const char *const ssid, const char *const passwd = NULL) {
  if (passwd)
    WiFi.begin(ssid, passwd);
  else
    WiFi.begin(ssid);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("\n");
  Serial.println("WiFi connected");
}

#endif
