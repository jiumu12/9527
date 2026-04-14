#include "communication/cmd_handler.h"
#include "config/config.h"
#include "light/light_engine.h"
#include "communication/com_server.h"
#include <ArduinoJson.h>

// 二进制协议命令定义
#define CMD_BINARY_STATIC 0x01
#define CMD_BINARY_EFFECT 0x02
#define CMD_BINARY_SWITCH 0x03
#define CMD_BINARY_PING 0x04
#define CMD_BINARY_GET_STATUS 0x05

// 效果类型映射
#define EFFECT_BINARY_STATIC 0
#define EFFECT_BINARY_BREATH 1
#define EFFECT_BINARY_RAINBOW 2
#define EFFECT_BINARY_FLASHING 3

void handleCommand(char* command) {
  // 检查是否为二进制协议
  if ((uint8_t)command[0] <= 0x05) {
    handleBinaryCommand((uint8_t*)command);
  } else {
    // 处理JSON协议
    DynamicJsonDocument doc(256);
    DeserializationError error = deserializeJson(doc, command);
    
    if (error) {
      Serial.printf("JSON parsing error: %s\n", error.c_str());
      return;
    }
    
    const char* cmd = doc["cmd"];
    
    if (strcmp(cmd, "static") == 0) {
      parseStaticCommand(command);
    } else if (strcmp(cmd, "effect") == 0) {
      parseEffectCommand(command);
    } else if (strcmp(cmd, "switch") == 0) {
      parseSwitchCommand(command);
    } else if (strcmp(cmd, "get_status") == 0) {
      parseGetStatusCommand();
    } else if (strcmp(cmd, "ping") == 0) {
      // 心跳包处理
    } else if (strcmp(cmd, "protocol_negotiate") == 0) {
      handleProtocolNegotiation();
    } else {
      Serial.printf("Unknown command: %s\n", cmd);
    }
  }
}

void handleBinaryCommand(uint8_t* data) {
  uint8_t cmd = data[0];
  
  switch (cmd) {
    case CMD_BINARY_STATIC: {
      // 二进制格式: [0x01, r, g, b, w, brightness]
      if (data[1] == 5) { // 确保数据长度正确
        Color color;
        color.r = data[2];
        color.g = data[3];
        color.b = data[4];
        color.w = data[5];
        color.brightness = data[6];
        setStaticColor(color);
      }
      break;
    }
    case CMD_BINARY_EFFECT: {
      // 二进制格式: [0x02, effect_type, speed, r, g, b, w, brightness]
      if (data[1] == 7) { // 确保数据长度正确
        uint8_t effectType = data[2];
        uint8_t speed = data[3];
        
        Color color;
        color.r = data[4];
        color.g = data[5];
        color.b = data[6];
        color.w = data[7];
        color.brightness = data[8];
        
        LedMode mode;
        switch (effectType) {
          case EFFECT_BINARY_BREATH:
            mode = MODE_BREATHING;
            break;
          case EFFECT_BINARY_RAINBOW:
            mode = MODE_RAINBOW;
            break;
          case EFFECT_BINARY_FLASHING:
            mode = MODE_FLASHING;
            break;
          default:
            mode = MODE_SOLID;
        }
        
        setEffect(mode, color, speed);
      }
      break;
    }
    case CMD_BINARY_SWITCH: {
      // 二进制格式: [0x03, on_state]
      if (data[1] == 1) { // 确保数据长度正确
        bool on = data[2] == 1;
        setPower(on);
      }
      break;
    }
    case CMD_BINARY_PING:
      // 心跳包，无需处理
      break;
    case CMD_BINARY_GET_STATUS:
      parseGetStatusCommand();
      break;
    default:
      Serial.printf("Unknown binary command: 0x%02X\n", cmd);
  }
}

void handleProtocolNegotiation() {
  // 发送支持的协议类型
  DynamicJsonDocument doc(128);
  doc["cmd"] = "protocol_supported";
  doc["json"] = true;
  doc["binary"] = true;
  
  String response;
  serializeJson(doc, response);
  broadcastMessage(response.c_str());
}

void parseStaticCommand(const char* json) {
  DynamicJsonDocument doc(256);
  deserializeJson(doc, json);
  
  Color color;
  color.r = doc["color"]["r"];
  color.g = doc["color"]["g"];
  color.b = doc["color"]["b"];
  color.w = doc["color"]["w"];
  color.brightness = doc["brightness"];
  
  setStaticColor(color);
  
  // 发送确认消息
  String response;
  doc["status"] = "ok";
  serializeJson(doc, response);
  broadcastMessage(response.c_str());
}

void parseEffectCommand(const char* json) {
  DynamicJsonDocument doc(256);
  deserializeJson(doc, json);
  
  const char* effectName = doc["name"];
  uint8_t speed = doc["speed"];
  
  Color color;
  color.r = doc["color"]["r"];
  color.g = doc["color"]["g"];
  color.b = doc["color"]["b"];
  color.w = doc["color"]["w"];
  color.brightness = doc["brightness"];
  
  LedMode mode;
  if (strcmp(effectName, "breath") == 0) {
    mode = MODE_BREATHING;
  } else if (strcmp(effectName, "rainbow") == 0) {
    mode = MODE_RAINBOW;
  } else if (strcmp(effectName, "flashing") == 0) {
    mode = MODE_FLASHING;
  } else {
    mode = MODE_SOLID;
  }
  
  setEffect(mode, color, speed);
  
  // 发送确认消息
  String response;
  doc["status"] = "ok";
  serializeJson(doc, response);
  broadcastMessage(response.c_str());
}

void parseSwitchCommand(const char* json) {
  DynamicJsonDocument doc(256);
  deserializeJson(doc, json);
  
  bool on = doc["on"];
  setPower(on);
  
  // 发送确认消息
  String response;
  doc["status"] = "ok";
  serializeJson(doc, response);
  broadcastMessage(response.c_str());
}

void parseGetStatusCommand() {
  EffectParams params = getCurrentStatus();
  
  DynamicJsonDocument doc(256);
  doc["cmd"] = "status";
  doc["on"] = params.on;
  doc["mode"] = params.mode;
  doc["color"]["r"] = params.color.r;
  doc["color"]["g"] = params.color.g;
  doc["color"]["b"] = params.color.b;
  doc["color"]["w"] = params.color.w;
  doc["brightness"] = params.color.brightness;
  doc["speed"] = params.speed;
  
  String response;
  serializeJson(doc, response);
  broadcastMessage(response.c_str());
}
