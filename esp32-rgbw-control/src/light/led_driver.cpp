#include "light/led_driver.h"
#include "config/config.h"
#include <FastLED.h>

CRGBW leds[LED_COUNT];

void initLED() {
  FastLED.addLeds<SK6812, LED_PIN, GRBW>(leds, LED_COUNT);
  FastLED.setBrightness(LED_BRIGHTNESS);
  Serial.println("LED initialized");
}

void setPixelColor(uint16_t n, Color color) {
  if (n < LED_COUNT) {
    float brightness = color.brightness / 100.0;
    leds[n].r = color.r * brightness;
    leds[n].g = color.g * brightness;
    leds[n].b = color.b * brightness;
    leds[n].w = color.w * brightness;
  }
}

void setAllPixels(Color color) {
  for (int i = 0; i < LED_COUNT; i++) {
    setPixelColor(i, color);
  }
}

void showPixels() {
  FastLED.show();
}
