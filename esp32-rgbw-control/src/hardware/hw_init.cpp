#include "hardware/hw_init.h"
#include "config/config.h"

void initHardware() {
  initSerial();
  initGPIO();
  Serial.println("Hardware initialized");
}

void initGPIO() {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  Serial.println("GPIO initialized");
}

void initSerial() {
  Serial.begin(115200);
  while (!Serial) {
    ; // 等待串口连接
  }
  Serial.println("Serial initialized");
}
