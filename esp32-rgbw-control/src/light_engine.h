#ifndef LIGHT_ENGINE_H
#define LIGHT_ENGINE_H

#include "config.h"

void initLightEngine();
void setStaticColor(Color color);
void setEffect(LedMode mode, Color color, uint8_t speed);
void setPower(bool on);
EffectParams getCurrentStatus();
void updateLight();
Color hsvToRgbw(uint16_t h, uint8_t s, uint8_t v);

#endif
