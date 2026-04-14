#ifndef LED_DRIVER_H
#define LED_DRIVER_H

#include "config/config.h"

void initLED();
void setPixelColor(uint16_t n, Color color);
void setAllPixels(Color color);
void showPixels();

#endif
