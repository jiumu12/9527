#include "hardware/real_hardware.h"

// GPIO 控制
void RealHardware::pinMode(uint8_t pin, uint8_t mode) {
    ::pinMode(pin, mode);
}

void RealHardware::digitalWrite(uint8_t pin, uint8_t value) {
    ::digitalWrite(pin, value);
}

int RealHardware::digitalRead(uint8_t pin) {
    return ::digitalRead(pin);
}

int RealHardware::analogRead(uint8_t pin) {
    return ::analogRead(pin);
}

// 时间管理
unsigned long RealHardware::millis() {
    return ::millis();
}

void RealHardware::delay(unsigned long ms) {
    ::delay(ms);
}

// WiFi 网络
void RealHardware::wifiMode(uint8_t mode) {
    WiFi.mode((wifi_mode_t)mode);
}

bool RealHardware::wifiBegin(const char* ssid, const char* password) {
    WiFi.begin(ssid, password);
    return WiFi.status() == WL_CONNECTED;
}

void RealHardware::wifiSoftAP(const char* ssid, const char* password) {
    WiFi.softAP(ssid, password);
}

String RealHardware::localIP() {
    return WiFi.localIP().toString();
}

String RealHardware::softAPIP() {
    return WiFi.softAPIP().toString();
}

int RealHardware::wifiStatus() {
    return WiFi.status();
}

// 串口通信
void RealHardware::serialBegin(unsigned long baud) {
    Serial.begin(baud);
}

size_t RealHardware::print(const char* str) {
    return Serial.print(str);
}

size_t RealHardware::println(const char* str) {
    return Serial.println(str);
}

size_t RealHardware::printf(const char* format, ...) {
    va_list args;
    va_start(args, format);
    size_t result = Serial.printf(format, args);
    va_end(args);
    return result;
}