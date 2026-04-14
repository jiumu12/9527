#ifndef CONFIG_H
#define CONFIG_H

// WiFi配置
#define WIFI_SSID "ESP32-RGBW"
#define WIFI_PASSWORD "12345678"

// LED灯带配置
#define LED_PIN_R 23
#define LED_PIN_G 22
#define LED_PIN_B 21
#define LED_PIN_W 19
#define LED_BRIGHTNESS 255
#define LED_COUNT 1

// 服务器配置
#define SERVER_PORT 80

// 模式定义
enum LedMode {
  MODE_SOLID,
  MODE_BREATHING,
  MODE_RAINBOW,
  MODE_FLASHING
};

// 颜色结构
typedef struct {
  uint8_t r;
  uint8_t g;
  uint8_t b;
  uint8_t w;
  uint8_t brightness;
} Color;

#endif
