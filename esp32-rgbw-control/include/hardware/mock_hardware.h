#ifndef MOCK_HARDWARE_H
#define MOCK_HARDWARE_H

#include "hardware_interface.h"

class MockHardware : public HardwareInterface {
public:
    // GPIO 控制
    void pinMode(uint8_t pin, uint8_t mode) override;
    void digitalWrite(uint8_t pin, uint8_t value) override;
    int digitalRead(uint8_t pin) override;
    int analogRead(uint8_t pin) override;
    
    // 时间管理
    unsigned long millis() override;
    void delay(unsigned long ms) override;
    
    // WiFi 网络
    void wifiMode(uint8_t mode) override;
    bool wifiBegin(const char* ssid, const char* password) override;
    void wifiSoftAP(const char* ssid, const char* password) override;
    String localIP() override;
    String softAPIP() override;
    int wifiStatus() override;
    
    // 串口通信
    void serialBegin(unsigned long baud) override;
    size_t print(const char* str) override;
    size_t println(const char* str) override;
    size_t printf(const char* format, ...) override;
};

#endif