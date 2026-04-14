#include "hw_init.h"
#include "network_mgr.h"
#include "com_server.h"
#include "light_engine.h"
#include <Arduino.h>

void setup() {
  // 初始化硬件
  initHardware();
  
  // 初始化网络
  initNetwork();
  
  // 初始化WebSocket服务器
  initWebSocketServer();
  
  // 初始化灯光引擎
  initLightEngine();
  
  Serial.println("System initialized");
}

void loop() {
  // 处理WebSocket事件
  webSocket.loop();
  
  // 更新灯光效果
  updateLight();
  
  // 检查网络状态
  checkNetworkStatus();
  
  // 小延迟，避免占用过多CPU
  delay(10);
}
