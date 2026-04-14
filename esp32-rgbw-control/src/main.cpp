#include "hw_init.h"
#include "network_mgr.h"
#include "com_server.h"
#include "light_engine.h"
#include <Arduino.h>
#include "esp_task_wdt.h"

void setup() {
  // 初始化硬件
  initHardware();
  
  // 初始化看门狗定时器
  esp_task_wdt_init(10, true); // 10秒超时
  esp_task_wdt_add(NULL);
  
  // 初始化网络
  initNetwork();
  
  // 初始化WebSocket服务器
  initWebSocketServer();
  
  // 初始化灯光引擎
  initLightEngine();
  
  Serial.println("System initialized");
}

void loop() {
  // 喂狗
  esp_task_wdt_reset();
  
  // 处理WebSocket事件
  webSocket.loop();
  
  // 更新灯光效果
  updateLight();
  
  // 检查网络状态
  checkNetworkStatus();
  
  // 小延迟，避免占用过多CPU
  delay(10);
}
