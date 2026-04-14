#include "hardware/hw_init.h"
#include "network/network_mgr.h"
#include "communication/com_server.h"
#include "communication/message_queue.h"
#include "light/light_engine.h"
#include "memory/memory_mgr.h"
#include <Arduino.h>
#include "esp_task_wdt.h"
#include <FreeRTOS.h>

// 网络任务
void networkTask(void *pvParameters) {
  unsigned long lastMemoryCheck = 0;
  
  while (true) {
    // 处理WebSocket事件
    webSocket.loop();
    
    // 处理Web服务器请求
    server.handleClient();
    
    // 检查网络状态
    checkNetworkStatus();
    
    // 处理消息队列
    processMessageQueue();
    
    // 每5秒打印一次内存使用情况
    unsigned long currentMillis = millis();
    if (currentMillis - lastMemoryCheck > 5000) {
      lastMemoryCheck = currentMillis;
      printMemoryUsage();
    }
    
    vTaskDelay(pdMS_TO_TICKS(10)); // 10ms延迟
  }
}

void setup() {
  // 初始化硬件
  initHardware();
  
  // 初始化看门狗定时器
  esp_task_wdt_init(10, true); // 10秒超时
  esp_task_wdt_add(NULL);
  
  // 初始化内存管理器
  initMemoryManager();
  
  // 初始化消息队列
  initMessageQueue();
  
  // 初始化网络
  initNetwork();
  
  // 初始化WebSocket服务器
  initWebSocketServer();
  
  // 初始化灯光引擎
  initLightEngine();
  
  // 创建网络任务
  xTaskCreate(
    networkTask,
    "NetworkTask",
    4096,
    NULL,
    4,
    NULL
  );
  
  Serial.println("System initialized");
}

void loop() {
  // 喂狗
  esp_task_wdt_reset();
  
  // 主循环只负责看门狗，其他任务由FreeRTOS管理
  vTaskDelay(pdMS_TO_TICKS(100)); // 100ms延迟
}
