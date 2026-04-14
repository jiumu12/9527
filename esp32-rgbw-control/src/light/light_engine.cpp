#include "light/light_engine.h"
#include "config/config.h"
#include "light/led_driver.h"
#include <Arduino.h>

EffectParams currentParams;
unsigned long lastUpdateTime = 0;
uint8_t breatheValue = 0;
bool breatheUp = true;
uint16_t rainbowHue = 0;

void initLightEngine() {
  initLED();
  currentParams.on = false;
  currentParams.mode = MODE_SOLID;
  currentParams.color.r = 0;
  currentParams.color.g = 0;
  currentParams.color.b = 0;
  currentParams.color.w = 0;
  currentParams.color.brightness = 100;
  currentParams.speed = 50;
  Serial.println("Light engine initialized");
}

void setStaticColor(Color color) {
  currentParams.mode = MODE_SOLID;
  currentParams.color = color;
  updateLight();
  Serial.println("Static color set");
}

void setEffect(LedMode mode, Color color, uint8_t speed) {
  currentParams.mode = mode;
  currentParams.color = color;
  currentParams.speed = speed;
  breatheValue = 0;
  breatheUp = true;
  rainbowHue = 0;
  Serial.println("Effect set");
}

void setPower(bool on) {
  currentParams.on = on;
  if (!on) {
    // 关闭灯光
    Color offColor = {0, 0, 0, 0, 0};
    setAllPixels(offColor);
    showPixels();
  } else {
    // 开启灯光
    updateLight();
  }
  Serial.printf("Power %s\n", on ? "on" : "off");
}

EffectParams getCurrentStatus() {
  return currentParams;
}

void updateLight() {
  if (!currentParams.on) {
    return;
  }

  unsigned long currentTime = millis();
  
  switch (currentParams.mode) {
    case MODE_SOLID:
      setAllPixels(currentParams.color);
      showPixels();
      break;
      
    case MODE_BREATHING:
      if (currentTime - lastUpdateTime > (100 - currentParams.speed) * 5) {
        lastUpdateTime = currentTime;
        
        if (breatheUp) {
          breatheValue++;
          if (breatheValue >= 255) {
            breatheUp = false;
          }
        } else {
          breatheValue--;
          if (breatheValue <= 0) {
            breatheUp = true;
          }
        }
        
        Color breatheColor = currentParams.color;
        float brightness = breatheValue / 255.0 * (currentParams.color.brightness / 100.0);
        breatheColor.r = currentParams.color.r * brightness;
        breatheColor.g = currentParams.color.g * brightness;
        breatheColor.b = currentParams.color.b * brightness;
        breatheColor.w = currentParams.color.w * brightness;
        
        setAllPixels(breatheColor);
        showPixels();
      }
      break;
      
    case MODE_RAINBOW:
      if (currentTime - lastUpdateTime > (100 - currentParams.speed) * 10) {
        lastUpdateTime = currentTime;
        rainbowHue += 5;
        if (rainbowHue >= 360) {
          rainbowHue = 0;
        }
        
        for (int i = 0; i < LED_COUNT; i++) {
          Color rainbowColor = hsvToRgbw(rainbowHue + i * 10, 255, 255);
          rainbowColor.brightness = currentParams.color.brightness;
          setPixelColor(i, rainbowColor);
        }
        showPixels();
      }
      break;
      
    case MODE_FLASHING:
      if (currentTime - lastUpdateTime > (100 - currentParams.speed) * 50) {
        lastUpdateTime = currentTime;
        
        static bool flashState = false;
        flashState = !flashState;
        
        if (flashState) {
          setAllPixels(currentParams.color);
        } else {
          Color offColor = {0, 0, 0, 0, 0};
          setAllPixels(offColor);
        }
        showPixels();
      }
      break;
  }
}

// HSV转RGBW
Color hsvToRgbw(uint16_t h, uint8_t s, uint8_t v) {
  Color color;
  uint8_t r, g, b;
  
  if (s == 0) {
    r = g = b = v;
  } else {
    h %= 360;
    uint8_t region = h / 60;
    uint8_t remainder = (h % 60) * 255 / 60;
    
    uint8_t p = (v * (255 - s)) / 255;
    uint8_t q = (v * (255 - (s * remainder) / 255)) / 255;
    uint8_t t = (v * (255 - (s * (255 - remainder)) / 255)) / 255;
    
    switch (region) {
      case 0: r = v; g = t; b = p; break;
      case 1: r = q; g = v; b = p; break;
      case 2: r = p; g = v; b = t; break;
      case 3: r = p; g = q; b = v; break;
      case 4: r = t; g = p; b = v; break;
      case 5: r = v; g = p; b = q; break;
    }
  }
  
  color.r = r;
  color.g = g;
  color.b = b;
  color.w = 0; // 白色通道单独控制
  color.brightness = 100;
  
  return color;
}
