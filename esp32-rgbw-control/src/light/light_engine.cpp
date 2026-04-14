#include "light/light_engine.h"
#include "config/config.h"
#include "light/led_driver.h"
#include <Arduino.h>
#include <FreeRTOS.h>
#include <semphr.h>

// 状态机上下文
LightStateContext lightContext;

// 状态处理函数
void handleSolidState(LightStateContext* context) {
  if (context->params.on) {
    setAllPixels(context->params.color);
    showPixels();
  }
}

void handleBreathingState(LightStateContext* context) {
  if (!context->params.on) return;
  
  unsigned long currentTime = millis();
  if (currentTime - context->lastUpdate > (100 - context->params.speed) * 5) {
    context->lastUpdate = currentTime;
    
    if (context->breatheUp) {
      context->breatheValue++;
      if (context->breatheValue >= 255) {
        context->breatheUp = false;
      }
    } else {
      context->breatheValue--;
      if (context->breatheValue <= 0) {
        context->breatheUp = true;
      }
    }
    
    Color breatheColor = context->params.color;
    float brightness = context->breatheValue / 255.0 * (context->params.color.brightness / 100.0);
    breatheColor.r = context->params.color.r * brightness;
    breatheColor.g = context->params.color.g * brightness;
    breatheColor.b = context->params.color.b * brightness;
    breatheColor.w = context->params.color.w * brightness;
    
    setAllPixels(breatheColor);
    showPixels();
  }
}

void handleRainbowState(LightStateContext* context) {
  if (!context->params.on) return;
  
  unsigned long currentTime = millis();
  if (currentTime - context->lastUpdate > (100 - context->params.speed) * 10) {
    context->lastUpdate = currentTime;
    context->rainbowHue += 5;
    if (context->rainbowHue >= 360) {
      context->rainbowHue = 0;
    }
    
    for (int i = 0; i < LED_COUNT; i++) {
      Color rainbowColor = hsvToRgbw(context->rainbowHue + i * 10, 255, 255);
      rainbowColor.brightness = context->params.color.brightness;
      setPixelColor(i, rainbowColor);
    }
    showPixels();
  }
}

void handleFlashingState(LightStateContext* context) {
  if (!context->params.on) return;
  
  unsigned long currentTime = millis();
  if (currentTime - context->lastUpdate > (100 - context->params.speed) * 50) {
    context->lastUpdate = currentTime;
    
    static bool flashState = false;
    flashState = !flashState;
    
    if (flashState) {
      setAllPixels(context->params.color);
    } else {
      Color offColor = {0, 0, 0, 0, 0};
      setAllPixels(offColor);
    }
    showPixels();
  }
}

// 状态机处理函数
void processLightState(LightStateContext* context) {
  switch (context->params.mode) {
    case MODE_SOLID:
      handleSolidState(context);
      break;
    case MODE_BREATHING:
      handleBreathingState(context);
      break;
    case MODE_RAINBOW:
      handleRainbowState(context);
      break;
    case MODE_FLASHING:
      handleFlashingState(context);
      break;
  }
}

void initLightEngine() {
  // 初始化LED
  initLED();
  
  // 初始化状态机上下文
  lightContext.params.on = false;
  lightContext.params.mode = MODE_SOLID;
  lightContext.params.color.r = 0;
  lightContext.params.color.g = 0;
  lightContext.params.color.b = 0;
  lightContext.params.color.w = 0;
  lightContext.params.color.brightness = 100;
  lightContext.params.speed = 50;
  lightContext.breatheValue = 0;
  lightContext.breatheUp = true;
  lightContext.rainbowHue = 0;
  lightContext.lastUpdate = 0;
  
  // 创建互斥锁
  lightContext.mutex = xSemaphoreCreateMutex();
  if (lightContext.mutex == NULL) {
    Serial.println("Failed to create mutex");
    return;
  }
  
  // 创建灯光控制任务
  xTaskCreate(
    lightTask,
    "LightTask",
    2048,
    NULL,
    5,
    NULL
  );
  
  Serial.println("Light engine initialized");
}

void setStaticColor(Color color) {
  if (xSemaphoreTake(lightContext.mutex, portMAX_DELAY) == pdTRUE) {
    lightContext.params.mode = MODE_SOLID;
    lightContext.params.color = color;
    xSemaphoreGive(lightContext.mutex);
    Serial.println("Static color set");
  }
}

void setEffect(LedMode mode, Color color, uint8_t speed) {
  if (xSemaphoreTake(lightContext.mutex, portMAX_DELAY) == pdTRUE) {
    lightContext.params.mode = mode;
    lightContext.params.color = color;
    lightContext.params.speed = speed;
    lightContext.breatheValue = 0;
    lightContext.breatheUp = true;
    lightContext.rainbowHue = 0;
    lightContext.lastUpdate = 0;
    xSemaphoreGive(lightContext.mutex);
    Serial.println("Effect set");
  }
}

void setPower(bool on) {
  if (xSemaphoreTake(lightContext.mutex, portMAX_DELAY) == pdTRUE) {
    lightContext.params.on = on;
    if (!on) {
      // 关闭灯光
      Color offColor = {0, 0, 0, 0, 0};
      setAllPixels(offColor);
      showPixels();
    }
    xSemaphoreGive(lightContext.mutex);
    Serial.printf("Power %s\n", on ? "on" : "off");
  }
}

EffectParams getCurrentStatus() {
  EffectParams params;
  if (xSemaphoreTake(lightContext.mutex, portMAX_DELAY) == pdTRUE) {
    params = lightContext.params;
    xSemaphoreGive(lightContext.mutex);
  }
  return params;
}

void updateLight() {
  // 由任务处理，此函数保留为兼容旧代码
}

// 灯光控制任务
void lightTask(void *pvParameters) {
  while (true) {
    if (xSemaphoreTake(lightContext.mutex, portMAX_DELAY) == pdTRUE) {
      processLightState(&lightContext);
      xSemaphoreGive(lightContext.mutex);
    }
    vTaskDelay(pdMS_TO_TICKS(10)); // 10ms延迟
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
