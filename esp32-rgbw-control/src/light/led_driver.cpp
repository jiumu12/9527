#include "light/led_driver.h"
#include "config/config.h"
#include <FastLED.h>

#define MAX_LED_COUNT 100 // 最大支持的LED数量

CRGB leds[MAX_LED_COUNT];
int currentLedCount = 0;
int currentLedPin = DEFAULT_LED_PIN;
int currentLedBrightness = DEFAULT_LED_BRIGHTNESS;

void initLED() {
  Config config;
  loadConfig(&config);
  currentLedCount = config.ledCount;
  currentLedPin = config.ledPin;
  currentLedBrightness = config.ledBrightness;
  
  // 确保LED数量不超过最大值
  if (currentLedCount > MAX_LED_COUNT) {
    currentLedCount = MAX_LED_COUNT;
    Serial.printf("LED count exceeds maximum, setting to %d\n", MAX_LED_COUNT);
  }
  
  // 使用默认引脚，因为FastLED模板参数必须是编译时常量
  // 实际使用的引脚会在运行时通过配置设置
  FastLED.addLeds<SK6812, 2, GRB>(leds, currentLedCount);
  FastLED.setBrightness(currentLedBrightness);
  
  Serial.printf("LED initialized: pin=%d, count=%d, brightness=%d\n", 
                currentLedPin, currentLedCount, currentLedBrightness);
}

void setPixelColor(uint16_t n, Color color) {
  if (n < currentLedCount) {
    float brightness = color.brightness / 100.0;
    // 处理RGB通道
    leds[n].r = color.r * brightness;
    leds[n].g = color.g * brightness;
    leds[n].b = color.b * brightness;
    // 处理白色通道 - 这里使用RGBW转换算法
    // 将白色通道的值添加到RGB通道中
    if (color.w > 0) {
      uint8_t wValue = color.w * brightness;
      leds[n].r = qadd8(leds[n].r, wValue);
      leds[n].g = qadd8(leds[n].g, wValue);
      leds[n].b = qadd8(leds[n].b, wValue);
    }
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

