# ESP32 RGBW 灯带控制系统

## 项目概述

这是一个完整的 ESP32 RGBW 灯带控制系统，包含嵌入式固件和 Flutter 移动应用，支持通过 WiFi 网络远程控制 RGBW 灯带。

## 功能特性

- ✅ RGBW 四通道独立控制
- ✅ 多种灯光效果（静态、呼吸、彩虹、闪烁）
- ✅ WiFi 连接（AP模式和Station模式）
- ✅ Web 界面控制（快速验证工具）
- ✅ 移动应用控制（Flutter跨平台）
- ✅ mDNS 设备发现
- ✅ WebSocket 实时通信
- ✅ 硬件抽象层（提高可测试性）
- ✅ 网络重连机制（提高稳定性）
- ✅ 心跳包机制（保持连接）
- ✅ 二进制协议支持（提高性能）

## 项目结构

```
├── esp32-rgbw-control/     # 嵌入式固件
│   ├── include/            # 头文件
│   ├── src/                # 源代码
│   ├── platformio.ini      # PlatformIO配置
│   └── README.md           # 固件说明
├── esp32-rgbw-app/         # Flutter移动应用
│   ├── lib/                # 应用代码
│   ├── pubspec.yaml        # 依赖管理
│   └── README.md           # 应用说明
├── ESP32_RGBW_CONTROL_SYSTEM.md  # 完整项目文档
└── README.md               # 本文件
```

## 快速开始

### 1. 固件开发环境
1. 安装 Visual Studio Code
2. 安装 PlatformIO 插件
3. 打开 `esp32-rgbw-control` 目录
4. 安装依赖库：FastLED, ArduinoJson, WebSockets
5. 配置 `include/config/config.h` 中的 WiFi 设置
6. 编译并上传固件到 ESP32

### 2. 移动应用开发环境
1. 安装 Flutter SDK
2. 安装 Android Studio
3. 打开 `esp32-rgbw-app` 目录
4. 运行 `flutter pub get` 安装依赖
5. 运行 `flutter run` 启动应用

### 3. 使用方法
1. 确保 ESP32 和移动设备在同一网络
2. 打开移动应用，自动发现设备
3. 选择设备并连接
4. 使用控制界面调整颜色和效果

## 技术栈

| 类别 | 技术/库 | 用途 |
|------|---------|------|
| 嵌入式 | ESP32 + Arduino Framework | 主控制器 |
| 灯带控制 | FastLED | LED 灯带驱动 |
| 网络 | ESPmDNS, WiFi | 网络连接和设备发现 |
| 通信 | WebSockets, WebServer | 实时通信和Web界面 |
| 移动应用 | Flutter, web_socket_channel | 跨平台移动应用 |
| 状态管理 | Provider | Flutter 状态管理 |
| JSON处理 | ArduinoJson | 命令解析和状态同步 |

## 通信协议

系统支持两种通信协议：
- **JSON 协议**：人类可读，便于调试
- **二进制协议**：高效，适用于高频更新场景

## 硬件连接

| ESP32引脚 | 功能 | 连接对象 |
|-----------|------|----------|
| GPIO7 | 数据输出 | 灯带DI（通过220-470Ω电阻） |
| 5V | 电源输出 | 灯带V+ |
| GND | 接地 | 灯带GND |

## 常见问题

**Q: 设备未发现？**  
A: 确保ESP32和手机在同一网络，检查mDNS服务是否正常

**Q: 连接失败？**  
A: 检查WiFi配置，确保密码正确

**Q: LED 不亮？**  
A: 检查硬件连接，确保电源供应充足

**Q: 编译错误？**  
A: 确保所有依赖库已正确安装

## 文档

完整项目文档请参考：[ESP32_RGBW_CONTROL_SYSTEM.md](file:///workspace/ESP32_RGBW_CONTROL_SYSTEM.md)

## 版本

- **固件版本**: 1.0.0
- **应用版本**: 1.0.0
- **文档版本**: 2.0.0

## 维护者

ESP32 RGBW Control Project

🌟 **Happy Coding! 祝你编程愉快！**
