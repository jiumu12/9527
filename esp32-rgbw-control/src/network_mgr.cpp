#include "network_mgr.h"
#include "config.h"
#include <Arduino.h>

void initNetwork() {
  connectWiFi();
  setupMDNS();
  Serial.println("Network initialized");
}

void connectWiFi() {
  WiFi.softAP(WIFI_SSID, WIFI_PASSWORD);
  Serial.println("WiFi AP started");
  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());
}

void setupMDNS() {
  if (!MDNS.begin(MDNS_NAME)) {
    Serial.println("Error setting up mDNS");
    return;
  }
  MDNS.addService("http", "tcp", 80);
  MDNS.addService("ws", "tcp", SERVER_PORT);
  Serial.println("mDNS setup completed");
}

void checkNetworkStatus() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected");
  }
  if (!MDNS.isRunning()) {
    Serial.println("mDNS not running");
  }
}
