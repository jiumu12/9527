#include "light/led_driver.h"
#include "config/config.h"
#include "network/network_mgr.h"
#include <FastLED.h>

#define MAX_LED_COUNT 100 // 最大支持的LED数量

CRGBW leds[MAX_LED_COUNT];
int currentLedCount = 0;
int currentLedPin = DEFAULT_LED_PIN;
int currentLedBrightness = DEFAULT_LED_BRIGHTNESS;

void initLED() {
  Config* config = getGlobalConfig();
  currentLedCount = config->ledCount;
  currentLedPin = config->ledPin;
  currentLedBrightness = config->ledBrightness;
  
  // 确保LED数量不超过最大值
  if (currentLedCount > MAX_LED_COUNT) {
    currentLedCount = MAX_LED_COUNT;
    Serial.printf("LED count exceeds maximum, setting to %d\n", MAX_LED_COUNT);
  }
  
  // 使用默认引脚，因为FastLED模板参数必须是编译时常量
  // 实际使用的引脚会在运行时通过配置设置
  FastLED.addLeds<SK6812, DEFAULT_LED_PIN, GRBW>(leds, currentLedCount);
  FastLED.setBrightness(currentLedBrightness);
  
  Serial.printf("LED initialized: pin=%d, count=%d, brightness=%d\n", 
                currentLedPin, currentLedCount, currentLedBrightness);
}

void setPixelColor(uint16_t n, Color color) {
  if (n < currentLedCount) {
    float brightness = color.brightness / 100.0;
    leds[n].r = color.r * brightness;
    leds[n].g = color.g * brightness;
    leds[n].b = color.b * brightness;
    leds[n].w = color.w * brightness;
  }
}

void setAllPixels(Color color) {
  for (int i = 0; i < currentLedCount; i++) {
    setPixelColor(i, color);
  }
}

void showPixels() {
  FastLED.show();
}

void updateLEDConfig() {
  // 重新初始化LED配置
  initLED();
}

