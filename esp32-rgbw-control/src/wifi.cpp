#include "wifi.h"
#include "config.h"
#include <Arduino.h>

void initWiFi() {
  WiFi.softAP(WIFI_SSID, WIFI_PASSWORD);
  Serial.println("WiFi AP started");
  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());
}

void checkWiFiStatus() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected");
  }
}
