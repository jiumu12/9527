#include "network_mgr.h"
#include "config.h"
#include <Arduino.h>

unsigned long lastReconnectAttempt = 0;
const unsigned long reconnectInterval = 5000; // 5秒重连间隔

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
  unsigned long currentMillis = millis();
  
  // 检查WiFi状态
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected");
    // 尝试重连
    if (currentMillis - lastReconnectAttempt > reconnectInterval) {
      lastReconnectAttempt = currentMillis;
      Serial.println("Attempting to reconnect WiFi...");
      connectWiFi();
    }
  }
  
  // 检查mDNS状态
  if (!MDNS.isRunning()) {
    Serial.println("mDNS not running");
    // 尝试重新启动mDNS
    if (currentMillis - lastReconnectAttempt > reconnectInterval) {
      lastReconnectAttempt = currentMillis;
      Serial.println("Attempting to restart mDNS...");
      setupMDNS();
    }
  }
}
