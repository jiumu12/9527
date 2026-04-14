# ESP32 RGBW 控制应用

这是一个使用 Flutter 开发的跨平台移动应用，用于控制 ESP32 RGBW 灯带系统。

## 功能特性

- ✅ 设备发现：通过 mDNS 自动发现局域网内的 ESP32 设备
- ✅ 实时控制：通过 WebSocket 与设备建立实时连接
- ✅ 颜色控制：支持 RGBW 四通道独立调节
- ✅ 效果选择：支持静态、呼吸、彩虹、闪烁等多种灯光效果
- ✅ 亮度调节：全局亮度控制
- ✅ 状态同步：实时同步设备状态

## 技术栈

- Flutter 3.0+
- web_socket_channel：WebSocket 通信
- flutter_colorpicker：颜色选择器
- provider：状态管理
- mdns：设备发现

## 开发环境搭建

1. 安装 Flutter SDK：https://flutter.dev/docs/get-started/install
2. 安装 Android Studio 或 Visual Studio Code
3. 克隆项目：`git clone <repository-url>`
4. 安装依赖：`flutter pub get`
5. 运行项目：`flutter run`

## 项目结构

```
esp32-rgbw-app/
├── lib/
│   ├── main.dart                    # 应用入口
│   ├── models/                      # 数据模型
│   │   ├── device.dart              # 设备模型
│   │   ├── color.dart               # 颜色模型
│   │   └── effect.dart              # 效果模型
│   ├── services/                    # 服务层
│   │   ├── discovery_service.dart   # 设备发现服务
│   │   └── websocket_service.dart   # WebSocket通信服务
│   ├── controllers/                 # 控制器
│   │   ├── device_controller.dart   # 设备管理控制器
│   │   └── light_controller.dart    # 灯光控制控制器
│   └── pages/                       # 页面
│       ├── device_list_page.dart    # 设备列表页
│       └── control_page.dart        # 控制页面
├── pubspec.yaml                     # 依赖管理
└── README.md                        # 项目说明
```

## 使用说明

1. 确保 ESP32 设备已连接到同一局域网
2. 打开应用，自动搜索设备
3. 选择要控制的设备
4. 使用控制界面调节颜色、效果和亮度

## 故障排查

### 设备未发现
- 确保 ESP32 设备已开机并连接到同一网络
- 检查 ESP32 的 mDNS 服务是否正常运行
- 尝试重启应用

### 连接失败
- 确保设备 IP 地址正确
- 检查网络连接是否稳定
- 尝试重启 ESP32 设备

### 控制无响应
- 检查 WebSocket 连接是否正常
- 确保 ESP32 设备正在运行
- 尝试重新连接设备

## 版本历史

- v1.0.0：初始版本，支持基本控制功能

## 开发团队

- 维护者：ESP32 RGBW Control Project
- 最后更新：2026-04-14
