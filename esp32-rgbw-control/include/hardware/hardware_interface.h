#ifndef HARDWARE_INTERFACE_H
#define HARDWARE_INTERFACE_H

#include <Arduino.h>

class HardwareInterface {
public:
    virtual ~HardwareInterface() = default;
    
    // GPIO 控制
    virtual void pinMode(uint8_t pin, uint8_t mode) = 0;
    virtual void digitalWrite(uint8_t pin, uint8_t value) = 0;
    virtual int digitalRead(uint8_t pin) = 0;
    virtual int analogRead(uint8_t pin) = 0;
    
    // 时间管理
    virtual unsigned long millis() = 0;
    virtual void delay(unsigned long ms) = 0;
    
    // WiFi 网络
    virtual void wifiMode(uint8_t mode) = 0;
    virtual bool wifiBegin(const char* ssid, const char* password) = 0;
    virtual void wifiSoftAP(const char* ssid, const char* password) = 0;
    virtual String localIP() = 0;
    virtual String softAPIP() = 0;
    virtual int wifiStatus() = 0;
    
    // 串口通信
    virtual void serialBegin(unsigned long baud) = 0;
    virtual size_t print(const char* str) = 0;
    virtual size_t println(const char* str) = 0;
    virtual size_t printf(const char* format, ...) = 0;
};

#endif