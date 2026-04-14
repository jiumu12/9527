#include "light/light_engine.h"
#include "config/config.h"
#include "light/led_driver.h"
#include <Arduino.h>
#include <FreeRTOS.h>
#include <semphr.h>

// 颜色查找表缓存 - 扩展为包含饱和度和亮度的三维查找表
#define HSV_TABLE_SIZE 360
#define SAT_TABLE_SIZE 5
#define VAL_TABLE_SIZE 5
Color hsvToRgbwTable[HSV_TABLE_SIZE][SAT_TABLE_SIZE][VAL_TABLE_SIZE];
bool hsvTableInitialized = false;

// 效果缓存
Color effectCache[100]; // 最大100个LED的缓存
bool effectCacheValid = false;
LedMode lastEffectMode = MODE_SOLID;
Color lastEffectColor;
uint8_t lastEffectSpeed = 0;

// 状态机上下文
LightStateContext lightContext;

// 初始化HSV到RGBW的查找表
void initHsvTable() {
  if (!hsvTableInitialized) {
    // 预计算不同饱和度和亮度的颜色值
    uint8_t satValues[] = {0, 64, 128, 192, 255};
    uint8_t valValues[] = {0, 64, 128, 192, 255};
    
    for (uint16_t h = 0; h < HSV_TABLE_SIZE; h++) {
      for (uint8_t sIndex = 0; sIndex < SAT_TABLE_SIZE; sIndex++) {
        for (uint8_t vIndex = 0; vIndex < VAL_TABLE_SIZE; vIndex++) {
          hsvToRgbwTable[h][sIndex][vIndex] = hsvToRgbw(h, satValues[sIndex], valValues[vIndex]);
        }
      }
    }
    hsvTableInitialized = true;
    Serial.println("HSV lookup table initialized");
  }
}

// 状态处理函数
void handleSolidState(LightStateContext* context) {
  if (context->params.on) {
    // 检查缓存是否有效
    if (!effectCacheValid || lastEffectMode != MODE_SOLID || 
        memcmp(&lastEffectColor, &context->params.color, sizeof(Color)) != 0) {
      setAllPixels(context->params.color);
      showPixels();
      
      // 更新缓存
      lastEffectMode = MODE_SOLID;
      lastEffectColor = context->params.color;
      effectCacheValid = true;
    }
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
    
    // 使用硬件FPU加速计算
    float brightness = (float)context->breatheValue / 255.0f * ((float)context->params.color.brightness / 100.0f);
    Color breatheColor;
    breatheColor.r = (uint8_t)((float)context->params.color.r * brightness);
    breatheColor.g = (uint8_t)((float)context->params.color.g * brightness);
    breatheColor.b = (uint8_t)((float)context->params.color.b * brightness);
    breatheColor.w = (uint8_t)((float)context->params.color.w * brightness);
    breatheColor.brightness = context->params.color.brightness;
    
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
    
    // 使用查找表加速颜色转换
    for (int i = 0; i < LED_COUNT; i++) {
      uint16_t hue = (context->rainbowHue + i * 10) % 360;
      Color rainbowColor = hsvToRgbwTable[hue];
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
  
  // 初始化HSV查找表
  initHsvTable();
  
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
    effectCacheValid = false; // 使缓存无效
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
    effectCacheValid = false; // 使缓存无效
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
      effectCacheValid = false; // 使缓存无效
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
    vTaskDelay(pdMS_TO_TICKS(5)); // 减少延迟，提高响应速度
  }
}

// 从查找表中获取颜色值
Color getColorFromLookupTable(uint16_t h, uint8_t s, uint8_t v) {
  // 确保色相在有效范围内
  h %= HSV_TABLE_SIZE;
  
  // 计算饱和度和亮度的索引
  uint8_t sIndex = min((s * (SAT_TABLE_SIZE - 1)) / 255, (uint8_t)(SAT_TABLE_SIZE - 1));
  uint8_t vIndex = min((v * (VAL_TABLE_SIZE - 1)) / 255, (uint8_t)(VAL_TABLE_SIZE - 1));
  
  return hsvToRgbwTable[h][sIndex][vIndex];
}

// HSV转RGBW - 优化版本
Color hsvToRgbw(uint16_t h, uint8_t s, uint8_t v) {
  // 优先使用查找表
  if (hsvTableInitialized) {
    return getColorFromLookupTable(h, s, v);
  }
  
  // 如果查找表未初始化，使用计算方法
  Color color;
  uint8_t r, g, b;
  
  if (s == 0) {
    r = g = b = v;
  } else {
    h %= 360;
    uint8_t region = h / 60;
    uint8_t remainder = (h % 60) * 255 / 60;
    
    // 使用位运算和整数运算优化
    uint16_t p = (uint16_t)v * (255 - s) / 255;
    uint16_t q = (uint16_t)v * (255 - ((uint16_t)s * remainder) / 255) / 255;
    uint16_t t = (uint16_t)v * (255 - ((uint16_t)s * (255 - remainder)) / 255) / 255;
    
    switch (region) {
      case 0: r = v; g = t; b = p; break;
      case 1: r = q; g = v; b = p; break;
      case 2: r = p; g = v; b = t; break;
      case 3: r = p; g = q; b = v; break;
      case 4: r = t; g = p; b = v; break;
      case 5: r = v; g = p; b = q; break;
    }
  }
  
  // 优化RGBW转换，利用白色通道提高亮度
  uint8_t min_rgb = min(r, min(g, b));
  color.r = r - min_rgb;
  color.g = g - min_rgb;
  color.b = b - min_rgb;
  color.w = min_rgb;
  color.brightness = 100;
  
  return color;
}
