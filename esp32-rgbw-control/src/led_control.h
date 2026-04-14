#ifndef LED_CONTROL_H
#define LED_CONTROL_H

#include "config.h"
#include <Arduino.h>

void initLED();
void setColor(Color color);
void setMode(LedMode mode);
void updateLED();

#endif
