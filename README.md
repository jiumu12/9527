# ESP32 玩具车项目 - SK6812 RGBW LED 控制

## 快速开始

### 1. 安装 Arduino IDE
下载并安装最新版本的 [Arduino IDE](https://www.arduino.cc/en/software)

### 2. 配置 ESP32 开发板
1. 打开 Arduino IDE
2. 文件 → 首选项 → 在"附加开发板管理器网址"中添加：
   ```
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
   ```
3. 工具 → 开发板 → 开发板管理器 → 搜索"esp32"并安装

### 3. 安装必要的库
1. 项目 → 加载库 → 管理库
2. 搜索 `Adafruit NeoPixel` 并安装

### 4. 上传代码
1. 打开 `esp32_toy_car.ino`
2. 选择开发板：工具 → 开发板 → ESP32 Arduino → ESP32 Dev Module
3. 选择端口：工具 → 端口 → 选择你的 ESP32 端口
4. 点击上传按钮

### 5. 使用控制界面
1. 连接 WiFi 热点：`ToyCar_XX`（密码：`12345678`）
2. 打开浏览器访问：`http://192.168.4.1`
3. 使用 Web 界面控制 LED

## 项目文件

```
esp32_toy_car/
├── esp32_toy_car.ino    # 主程序
├── config.h             # 配置文件
├── led_controller.h     # LED 控制器头文件
├── led_controller.cpp   # LED 控制器实现
├── webserver.h          # Web 服务器头文件
├── webserver.cpp        # Web 服务器实现
├── wifi_manager.h       # WiFi 管理器头文件
├── wifi_manager.cpp     # WiFi 管理器实现
├── html_content.h       # Web 界面内容
└── README.md            # 说明文档
```

## 功能特性

- ✅ RGBW 四通道独立控制
- ✅ 常亮、呼吸灯、流水灯三种模式
- ✅ 流水灯支持：单颗、堆叠、循环堆叠
- ✅ 可配置流动方向和数量
- ✅ 圆形色环颜色选择器
- ✅ 实时颜色预览
- ✅ 动画暂停/恢复机制

## 配置说明

编辑 `config.h` 文件可修改：

```cpp
#define LED_PIN 2              // LED 数据引脚
#define NUM_LEDS 10            // LED 数量
#define LED_BRIGHTNESS 255     // 默认亮度 (0-255)
#define WIFI_SSID "ToyCar_"    // WiFi 名称
#define WIFI_PASSWORD "12345678"  // WiFi 密码
```

## 常见问题

**Q: 上传失败？**  
A: 检查 USB 线连接，安装正确的驱动（CH340 或 CP210x）

**Q: LED 颜色不对？**  
A: 在 Web 界面中尝试不同的通道顺序设置

**Q: 编译错误？**  
A: 确保已安装 Adafruit NeoPixel 库

祝你使用愉快！
