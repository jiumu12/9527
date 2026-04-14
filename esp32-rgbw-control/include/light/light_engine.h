#ifndef LIGHT_ENGINE_H
#define LIGHT_ENGINE_H

#include "config/config.h"
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

// 灯光状态上下文结构体
typedef struct {
  EffectParams params;       // 效果参数
  uint8_t breatheValue;      // 呼吸效果当前值
  bool breatheUp;            // 呼吸效果方向
  uint16_t rainbowHue;       // 彩虹效果当前色相
  unsigned long lastUpdate;  // 上次更新时间
  SemaphoreHandle_t mutex;    // 互斥锁
} LightStateContext;

void initLightEngine();
void setStaticColor(Color color);
void setEffect(LedMode mode, Color color, uint8_t speed);
void setPower(bool on);
EffectParams getCurrentStatus();
void updateLight();
Color hsvToRgbw(uint16_t h, uint8_t s, uint8_t v);
Color getColorFromLookupTable(uint16_t h, uint8_t s, uint8_t v);
void lightTask(void *pvParameters);

#endif
