#ifndef CONFIG_H
#define CONFIG_H

// WiFi配置
#define WIFI_MODE 1  // 0: AP模式, 1: Station模式

// AP模式配置
#define AP_SSID "ESP32-RGBW"
#define AP_PASSWORD "12345678"

// Station模式配置（连接到路由器）
#define STATION_SSID "YourRouterSSID"  // 替换为你的路由器SSID
#define STATION_PASSWORD "YourRouterPassword"  // 替换为你的路由器密码

// LED灯带配置
#define LED_PIN 7                   // 推荐使用GPIO7
#define LED_TYPE SK6812             // 灯带类型：SK6812, WS2812B, WS2815
#define LED_COUNT 30                // 灯珠数量
#define LED_BRIGHTNESS 255          // 最大亮度

// 服务器配置
#define SERVER_PORT 8080            // WebSocket服务器端口
#define MDNS_NAME "esp32-rgbw"     // mDNS服务名称

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

#endif
