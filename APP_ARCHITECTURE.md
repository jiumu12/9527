# ESP32 RGBW LED Control App Architecture

## 项目概述
本移动应用旨在通过WebSocket与ESP32设备通信，实现对RGBW灯带的远程控制。

## 技术选型
- **框架**：Flutter（跨平台支持）
- **语言**：Dart
- **通信**：WebSocket（web_socket_channel库）
- **UI组件**：flutter_colorpicker, provider
- **网络发现**：mdns

## 目录结构

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
│   │   ├── websocket_service.dart   # WebSocket通信服务
│   │   └── color_service.dart       # 颜色处理服务
│   ├── controllers/                 # 控制器
│   │   ├── device_controller.dart   # 设备管理控制器
│   │   └── light_controller.dart    # 灯光控制控制器
│   ├── pages/                       # 页面
│   │   ├── device_list_page.dart    # 设备列表页
│   │   └── control_page.dart        # 控制页面
│   ├── widgets/                     # 组件
│   │   ├── color_picker.dart        # 颜色选择器
│   │   ├── effect_selector.dart     # 效果选择器
│   │   ├── brightness_slider.dart   # 亮度滑块
│   │   └── temperature_slider.dart  # 色温滑块
│   └── utils/                       # 工具类
│       ├── constants.dart           # 常量定义
│       └── helpers.dart             # 辅助函数
├── pubspec.yaml                     # 依赖管理
└── README.md                        # 项目说明
```

## 核心功能模块

### 1. 设备发现模块
- **功能**：通过mDNS扫描局域网内的ESP32设备
- **实现**：使用mdns库，搜索特定服务类型
- **流程**：
  1. 启动应用时自动扫描
  2. 显示发现的设备列表
  3. 支持手动刷新

### 2. 通信模块
- **功能**：与ESP32建立WebSocket连接，收发数据
- **实现**：使用web_socket_channel库
- **流程**：
  1. 用户选择设备后建立连接
  2. 发送控制命令
  3. 接收设备状态更新
  4. 处理连接异常

### 3. 颜色控制模块
- **功能**：提供多种颜色选择方式
- **实现**：
  - 色环调色：使用flutter_colorpicker库
  - 色盘拾色：自定义组件
  - RGBW输入框：带滑块和数字输入
  - W通道独立调节：单独滑块

### 4. 效果控制模块
- **功能**：切换不同的灯光效果
- **实现**：
  - 静态模式：固定颜色
  - 呼吸模式：可调节速度
  - 彩虹模式：自动颜色变化
  - 闪烁模式：可调节速度

### 5. 亮度和色温控制
- **功能**：调节整体亮度和色温
- **实现**：
  - 亮度滑块：0-100%
  - 色温滑块：2000K-6500K
  - 动态计算RGBW值

## 用户界面设计

### 1. 设备列表页面
- **布局**：列表形式展示发现的设备
- **功能**：
  - 显示设备名称和IP地址
  - 点击设备进入控制页面
  - 支持手动刷新设备列表
  - 显示连接状态

### 2. 控制页面
- **布局**：
  - 顶部：设备名称和总开关
  - 中部：颜色选择区域
  - 底部：效果选择和参数调节

- **颜色选择区域**：
  - 色环调色器
  - RGBW输入框（带滑块）
  - W通道独立调节器
  - 亮度滑块
  - 色温滑块

- **效果选择区域**：
  - 效果类型选择（静态、呼吸、彩虹、闪烁）
  - 效果速度调节
  - 效果颜色选择

## 通信协议

### 发送命令格式

#### 设置静态颜色
```json
{
  "cmd": "static",
  "color": {
    "r": 255,
    "g": 100,
    "b": 50,
    "w": 0
  },
  "brightness": 80
}
```

#### 设置效果
```json
{
  "cmd": "effect",
  "name": "breath",
  "speed": 50,
  "color": {
    "r": 255,
    "g": 0,
    "b": 0,
    "w": 0
  },
  "brightness": 80
}
```

#### 开关控制
```json
{
  "cmd": "switch",
  "on": true
}
```

#### 获取状态
```json
{
  "cmd": "get_status"
}
```

### 接收状态格式
```json
{
  "cmd": "status",
  "on": true,
  "mode": 0,
  "color": {
    "r": 255,
    "g": 100,
    "b": 50,
    "w": 0
  },
  "brightness": 80,
  "speed": 50
}
```

## 状态管理

使用Provider进行状态管理：

1. **DeviceProvider**：管理设备列表和当前连接状态
2. **LightProvider**：管理灯光状态、颜色和效果

## 错误处理

1. **连接错误**：
   - 显示连接失败提示
   - 自动重连机制
   - 超时处理

2. **命令执行错误**：
   - 显示错误信息
   - 恢复到之前状态

3. **设备离线**：
   - 检测设备在线状态
   - 显示离线提示

## 性能优化

1. **WebSocket连接**：
   - 心跳机制保持连接
   - 断线重连

2. **UI响应**：
   - 使用防抖处理滑块输入
   - 批量发送命令

3. **电量优化**：
   - 后台限制网络扫描频率
   - 连接状态监测

## 扩展性考虑

1. **多设备控制**：
   - 支持同时控制多个ESP32设备
   - 设备分组管理

2. **场景模式**：
   - 保存和加载预设场景
   - 定时任务

3. **语音控制**：
   - 集成语音助手

4. **远程控制**：
   - 云端接入
   - 远程访问

## 开发步骤

1. **环境搭建**：
   - 安装Flutter SDK
   - 配置开发环境

2. **基础框架**：
   - 创建项目结构
   - 实现基本页面布局

3. **设备发现**：
   - 实现mDNS扫描
   - 设备列表管理

4. **通信模块**：
   - WebSocket连接管理
   - 命令发送和接收

5. **UI组件**：
   - 颜色选择器
   - 效果选择器
   - 滑块组件

6. **状态管理**：
   - Provider实现
   - 状态同步

7. **测试和优化**：
   - 功能测试
   - 性能优化
   - 兼容性测试

## 技术依赖

| 依赖 | 版本 | 用途 |
|------|------|------|
| flutter | ^3.0.0 | 基础框架 |
| web_socket_channel | ^2.2.0 | WebSocket通信 |
| flutter_colorpicker | ^1.0.3 | 颜色选择器 |
| provider | ^6.0.3 | 状态管理 |
| mdns | ^0.3.0 | 设备发现 |
| cupertino_icons | ^1.0.2 | UI图标 |
