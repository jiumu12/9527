#include "config/config.h"

Preferences preferences;

void loadConfig(Config* config) {
  preferences.begin("esp32-rgbw", false);
  
  // 加载WiFi配置
  config->wifiMode = preferences.getInt(PREF_WIFI_MODE, DEFAULT_WIFI_MODE);
  config->apSSID = preferences.getString(PREF_AP_SSID, DEFAULT_AP_SSID);
  config->apPassword = preferences.getString(PREF_AP_PASSWORD, DEFAULT_AP_PASSWORD);
  config->stationSSID = preferences.getString(PREF_STATION_SSID, DEFAULT_STATION_SSID);
  config->stationPassword = preferences.getString(PREF_STATION_PASSWORD, DEFAULT_STATION_PASSWORD);
  
  // 加载LED配置
  config->ledPin = preferences.getInt(PREF_LED_PIN, DEFAULT_LED_PIN);
  config->ledCount = preferences.getInt(PREF_LED_COUNT, DEFAULT_LED_COUNT);
  config->ledBrightness = preferences.getInt(PREF_LED_BRIGHTNESS, DEFAULT_LED_BRIGHTNESS);
  
  preferences.end();
  
  Serial.println("Configuration loaded:");
  Serial.printf("WiFi Mode: %d\n", config->wifiMode);
  Serial.printf("AP SSID: %s\n", config->apSSID.c_str());
  Serial.printf("Station SSID: %s\n", config->stationSSID.c_str());
  Serial.printf("LED Pin: %d\n", config->ledPin);
  Serial.printf("LED Count: %d\n", config->ledCount);
  Serial.printf("LED Brightness: %d\n", config->ledBrightness);
}

void saveConfig(Config* config) {
  preferences.begin("esp32-rgbw", false);
  
  // 保存WiFi配置
  preferences.putInt(PREF_WIFI_MODE, config->wifiMode);
  preferences.putString(PREF_AP_SSID, config->apSSID);
  preferences.putString(PREF_AP_PASSWORD, config->apPassword);
  preferences.putString(PREF_STATION_SSID, config->stationSSID);
  preferences.putString(PREF_STATION_PASSWORD, config->stationPassword);
  
  // 保存LED配置
  preferences.putInt(PREF_LED_PIN, config->ledPin);
  preferences.putInt(PREF_LED_COUNT, config->ledCount);
  preferences.putInt(PREF_LED_BRIGHTNESS, config->ledBrightness);
  
  preferences.end();
  
  Serial.println("Configuration saved:");
  Serial.printf("WiFi Mode: %d\n", config->wifiMode);
  Serial.printf("AP SSID: %s\n", config->apSSID.c_str());
  Serial.printf("Station SSID: %s\n", config->stationSSID.c_str());
  Serial.printf("LED Pin: %d\n", config->ledPin);
  Serial.printf("LED Count: %d\n", config->ledCount);
  Serial.printf("LED Brightness: %d\n", config->ledBrightness);
}

void resetConfig() {
  preferences.begin("esp32-rgbw", false);
  preferences.clear();
  preferences.end();
  
  Serial.println("Configuration reset to defaults");
}
