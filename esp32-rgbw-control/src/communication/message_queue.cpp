#include "communication/message_queue.h"
#include "communication/cmd_handler.h"
#include "memory/memory_mgr.h"
#include <Arduino.h>

// 消息队列句柄
static QueueHandle_t messageQueue;

void initMessageQueue() {
  // 创建消息队列，最大队列长度为10
  messageQueue = xQueueCreate(10, sizeof(Message));
  if (messageQueue == NULL) {
    Serial.println("Failed to create message queue");
  } else {
    Serial.println("Message queue initialized");
  }
}

bool sendMessage(MessageType type, const char* data, size_t dataLength) {
  if (messageQueue == NULL) {
    Serial.println("Message queue not initialized");
    return false;
  }
  
  // 分配内存存储消息数据
  char* dataCopy = (char*)allocateMemory(dataLength + 1);
  if (dataCopy == NULL) {
    Serial.println("Failed to allocate memory for message");
    return false;
  }
  
  // 复制数据
  memcpy(dataCopy, data, dataLength);
  dataCopy[dataLength] = '\0';
  
  // 创建消息
  Message message;
  message.type = type;
  message.data = dataCopy;
  message.dataLength = dataLength;
  
  // 发送消息到队列
  if (xQueueSend(messageQueue, &message, pdMS_TO_TICKS(100)) != pdPASS) {
    Serial.println("Failed to send message to queue");
    freeMemory(dataCopy);
    return false;
  }
  
  Serial.printf("Message sent to queue: type=%d, length=%u\n", type, dataLength);
  return true;
}

bool receiveMessage(Message* message) {
  if (messageQueue == NULL) {
    return false;
  }
  
  // 从队列接收消息
  if (xQueueReceive(messageQueue, message, pdMS_TO_TICKS(100)) == pdPASS) {
    return true;
  }
  
  return false;
}

void processMessageQueue() {
  Message message;
  
  // 处理队列中的所有消息
  while (receiveMessage(&message)) {
    switch (message.type) {
      case MSG_TYPE_COMMAND:
        // 处理命令消息
        Serial.println("Processing command message");
        handleCommand(message.data);
        break;
      case MSG_TYPE_STATUS:
        // 处理状态消息
        Serial.println("Processing status message");
        // 这里可以添加状态消息的处理逻辑
        break;
      case MSG_TYPE_ERROR:
        // 处理错误消息
        Serial.println("Processing error message");
        // 这里可以添加错误消息的处理逻辑
        break;
    }
    
    // 释放消息数据内存
    if (message.data != NULL) {
      freeMemory(message.data);
    }
  }
}