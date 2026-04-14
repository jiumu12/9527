#include "network/network_mgr.h"
#include "config/config.h"
#include <Arduino.h>

unsigned long lastReconnectAttempt = 0;
const unsigned long reconnectInterval = 5000; // 5秒重连间隔
Config globalConfig;

void initNetwork() {
  // 加载配置
  loadConfig(&globalConfig);
  
  connectWiFi();
  setupMDNS();
  Serial.println("Network initialized");
}

void connectWiFi() {
  if (globalConfig.wifiMode == 0) {
    // AP模式
    WiFi.softAP(globalConfig.apSSID.c_str(), globalConfig.apPassword.c_str());
    Serial.println("WiFi AP started");
    Serial.print("AP IP address: ");
    Serial.println(WiFi.softAPIP());
  } else {
    // Station模式（连接到路由器）
    if (globalConfig.stationSSID.length() > 0) {
      WiFi.mode(WIFI_STA);
      WiFi.begin(globalConfig.stationSSID.c_str(), globalConfig.stationPassword.c_str());
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
        WiFi.softAP(globalConfig.apSSID.c_str(), globalConfig.apPassword.c_str());
        Serial.println("Fallback to AP mode");
        Serial.print("AP IP address: ");
        Serial.println(WiFi.softAPIP());
      }
    } else {
      // 没有配置Station模式，使用AP模式
      WiFi.softAP(globalConfig.apSSID.c_str(), globalConfig.apPassword.c_str());
      Serial.println("No station config, using AP mode");
      Serial.print("AP IP address: ");
      Serial.println(WiFi.softAPIP());
    }
  }
}

void setupMDNS() {
  if (!MDNS.begin("esp32-rgbw")) {
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
  if (globalConfig.wifiMode == 1) {
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
  }
  
  // 检查mDNS状态 - 移除isRunning()检查，因为ESP32 MDNS库可能没有这个方法
  // 定期重新启动mDNS以确保它正常运行
  static unsigned long lastMDNSCheck = 0;
  if (currentMillis - lastMDNSCheck > 60000) { // 每分钟检查一次
    lastMDNSCheck = currentMillis;
    // 重新启动mDNS
    setupMDNS();
  }
}

Config* getGlobalConfig() {
  return &globalConfig;
}

void updateConfig(Config* newConfig) {
  globalConfig = *newConfig;
  saveConfig(&globalConfig);
  // 重新初始化网络
  initNetwork();
}

