#ifndef CONFIG_H
#define CONFIG_H

#include <Preferences.h>

// 配置存储键名
#define PREF_WIFI_MODE "wifi_mode"
#define PREF_AP_SSID "ap_ssid"
#define PREF_AP_PASSWORD "ap_password"
#define PREF_STATION_SSID "station_ssid"
#define PREF_STATION_PASSWORD "station_password"
#define PREF_LED_PIN "led_pin"
#define PREF_LED_COUNT "led_count"
#define PREF_LED_BRIGHTNESS "led_brightness"

// 默认配置
#define DEFAULT_WIFI_MODE 0  // 0: AP模式, 1: Station模式
#define DEFAULT_AP_SSID "ESP32-RGBW"
#define DEFAULT_AP_PASSWORD "12345678"
#define DEFAULT_STATION_SSID ""
#define DEFAULT_STATION_PASSWORD ""
#define DEFAULT_LED_PIN 7                   // 推荐使用GPIO7
#define DEFAULT_LED_COUNT 30                // 灯珠数量
#define LED_COUNT DEFAULT_LED_COUNT         // LED 数量宏
#define DEFAULT_LED_BRIGHTNESS 255          // 最大亮度

// 服务器配置
#define SERVER_PORT 8080            // WebSocket服务器端口
#ifndef MDNS_NAME
#define MDNS_NAME "esp32-rgbw"     // mDNS服务名称
#endif

// 模式定义
enum LedMode {
  MODE_SOLID,                       // 静态颜色
  MODE_BREATHING,                   // 呼吸效果
  MODE_RAINBOW,                     // 彩虹效果
  MODE_FLASHING                     // 闪烁效果
};

// 颜色结构
typedef struct {
  uint8_t r;
  uint8_t g;
  uint8_t b;
  uint8_t w;
  uint8_t brightness;
} Color;

// 效果参数结构
typedef struct {
  LedMode mode;
  Color color;
  uint8_t speed;                    // 效果速度 (0-100)
  bool on;                          // 开关状态
} EffectParams;

// 配置结构
typedef struct {
  int wifiMode;
  String apSSID;
  String apPassword;
  String stationSSID;
  String stationPassword;
  int ledPin;
  int ledCount;
  int ledBrightness;
} Config;

// 函数声明
void loadConfig(Config* config);
void saveConfig(Config* config);
void resetConfig();

#endif
