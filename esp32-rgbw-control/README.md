# ESP32 RGBW 灯带控制固件

这是一个基于 ESP32 的 RGBW 灯带控制固件，支持通过 WebSocket 和 Web 界面控制灯带。

## 功能特性

- ✅ RGBW 四通道独立控制
- ✅ 多种灯光效果（静态、呼吸、彩虹、闪烁）
- ✅ WiFi 连接（AP模式和Station模式）
- ✅ Web 界面控制（快速验证工具）
- ✅ WebSocket 实时通信
- ✅ mDNS 设备发现
- ✅ 硬件抽象层（提高可测试性）
- ✅ 网络重连机制（提高稳定性）
- ✅ 心跳包机制（保持连接）

## 技术栈

- ESP32 + Arduino Framework
- FastLED：LED 灯带驱动
- WebSockets：实时通信
- WebServer：Web 界面
- ArduinoJson：JSON 解析
- ESPmDNS：设备发现

## 开发环境搭建

1. 安装 Visual Studio Code
2. 安装 PlatformIO 插件
3. 克隆项目：`git clone <repository-url>`
4. 打开项目文件夹
5. 安装依赖库（PlatformIO 会自动处理）
6. 编译并上传固件

## 项目结构

```
esp32-rgbw-control/
├── platformio.ini          # PlatformIO配置文件
├── README.md               # 项目说明
└── src/
    ├── main.cpp            # 主程序
    ├── config.h            # 配置文件
    ├── hardware_interface.h # 硬件抽象层接口
    ├── real_hardware.cpp   # 真实硬件实现
    ├── real_hardware.h
    ├── mock_hardware.h     # 模拟硬件实现（用于测试）
    ├── hw_init.cpp         # 硬件初始化模块
    ├── hw_init.h
    ├── network_mgr.cpp     # 网络连接模块
    ├── network_mgr.h
    ├── com_server.cpp      # 通信服务器模块（包含Web界面）
    ├── com_server.h
    ├── cmd_handler.cpp     # 协议解析与命令处理模块
    ├── cmd_handler.h
    ├── light_engine.cpp    # 灯光引擎核心模块
    ├── light_engine.h
    ├── led_driver.cpp      # 灯带驱动抽象层
    └── led_driver.h
```

## 配置说明

编辑 `config.h` 文件可修改以下配置：

- WiFi 模式（AP模式或Station模式）
- WiFi 凭证（SSID和密码）
- LED 引脚、类型和数量
- 服务器端口和 mDNS 名称

## 硬件连接

- **GPIO7**：连接到灯带的 DI 引脚（通过 220-470Ω 电阻）
- **5V**：连接到灯带的 V+ 引脚
- **GND**：连接到灯带的 GND 引脚

## 使用说明

### Web 界面控制
1. 确保 ESP32 设备已连接到网络
2. 打开浏览器访问：`http://esp32-rgbw.local`
3. 使用 Web 界面控制灯带

### 移动应用控制
1. 安装 ESP32 RGBW 控制应用
2. 确保手机与 ESP32 在同一网络
3. 打开应用，自动发现设备
4. 选择设备并开始控制

## 故障排查

### 编译错误
- 确保已安装所有依赖库
- 检查 PlatformIO 配置是否正确

### 网络连接问题
- 检查 WiFi 凭证是否正确
- 确保路由器信号强度足够
- 尝试重启 ESP32 设备

### 灯光控制问题
- 检查硬件连接是否正确
- 确保电源供应充足
- 检查 LED 灯带类型是否正确配置

## 版本历史

- v1.0.0：初始版本，支持基本控制功能

## 开发团队

- 维护者：ESP32 RGBW Control Project
- 最后更新：2026-04-14
