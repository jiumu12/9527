#include "network/network_mgr.h"
#include "config/config.h"
#include <Arduino.h>

unsigned long lastReconnectAttempt = 0;
const unsigned long reconnectInterval = 5000; // 5秒重连间隔

void initNetwork() {
  connectWiFi();
  setupMDNS();
  Serial.println("Network initialized");
}

void connectWiFi() {
  #if WIFI_MODE == 0
    // AP模式
    WiFi.softAP(AP_SSID, AP_PASSWORD);
    Serial.println("WiFi AP started");
    Serial.print("AP IP address: ");
    Serial.println(WiFi.softAPIP());
  #else
    // Station模式（连接到路由器）
    WiFi.mode(WIFI_STA);
    WiFi.begin(STATION_SSID, STATION_PASSWORD);
    Serial.println("Connecting to WiFi...");
    
    // 等待连接
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
      delay(500);
      Serial.print(".");
      attempts++;
    }
    
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("WiFi connected");
      Serial.print("IP address: ");
      Serial.println(WiFi.localIP());
    } else {
      Serial.println("Failed to connect to WiFi");
      // 如果连接失败，回退到AP模式
      WiFi.softAP(AP_SSID, AP_PASSWORD);
      Serial.println("Fallback to AP mode");
      Serial.print("AP IP address: ");
      Serial.println(WiFi.softAPIP());
    }
  #endif
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
  #if WIFI_MODE == 1
    // Station模式
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("WiFi not connected");
      // 尝试重连
      if (currentMillis - lastReconnectAttempt > reconnectInterval) {
        lastReconnectAttempt = currentMillis;
        Serial.println("Attempting to reconnect WiFi...");
        connectWiFi();
      }
    }
  #endif
  
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
