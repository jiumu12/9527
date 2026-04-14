#ifndef MESSAGE_QUEUE_H
#define MESSAGE_QUEUE_H

#include <FreeRTOS.h>
#include <queue.h>
#include "config/config.h"

// 消息类型
typedef enum {
  MSG_TYPE_COMMAND,      // 命令消息
  MSG_TYPE_STATUS,       // 状态消息
  MSG_TYPE_ERROR         // 错误消息
} MessageType;

// 消息结构
typedef struct {
  MessageType type;      // 消息类型
  char* data;            // 消息数据
  size_t dataLength;     // 数据长度
} Message;

// 函数声明
void initMessageQueue();
bool sendMessage(MessageType type, const char* data, size_t dataLength);
bool receiveMessage(Message* message);
void processMessageQueue();

#endif // MESSAGE_QUEUE_H