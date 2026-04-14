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
      // 发送错误响应
      DynamicJsonDocument errorDoc(128);
      errorDoc["cmd"] = "error";
      errorDoc["message"] = "Invalid JSON format";
      String errorResponse;
      serializeJson(errorDoc, errorResponse);
      broadcastMessage(errorResponse.c_str());
      return;
    }
    
    if (!doc.containsKey("cmd")) {
      Serial.println("Missing 'cmd' field");
      // 发送错误响应
      DynamicJsonDocument errorDoc(128);
      errorDoc["cmd"] = "error";
      errorDoc["message"] = "Missing 'cmd' field";
      String errorResponse;
      serializeJson(errorDoc, errorResponse);
      broadcastMessage(errorResponse.c_str());
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
      DynamicJsonDocument pingDoc(64);
      pingDoc["cmd"] = "pong";
      String pingResponse;
      serializeJson(pingDoc, pingResponse);
      broadcastMessage(pingResponse.c_str());
    } else if (strcmp(cmd, "protocol_negotiate") == 0) {
      handleProtocolNegotiation();
    } else {
      Serial.printf("Unknown command: %s\n", cmd);
      // 发送错误响应
      DynamicJsonDocument errorDoc(128);
      errorDoc["cmd"] = "error";
      errorDoc["message"] = "Unknown command";
      String errorResponse;
      serializeJson(errorDoc, errorResponse);
      broadcastMessage(errorResponse.c_str());
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
  DeserializationError error = deserializeJson(doc, json);
  
  if (error) {
    Serial.printf("JSON parsing error: %s\n", error.c_str());
    return;
  }
  
  // 检查必要字段
  if (!doc.containsKey("color")) {
    Serial.println("Missing 'color' field");
    DynamicJsonDocument errorDoc(128);
    errorDoc["cmd"] = "error";
    errorDoc["message"] = "Missing 'color' field";
    String errorResponse;
    serializeJson(errorDoc, errorResponse);
    broadcastMessage(errorResponse.c_str());
    return;
  }
  
  JsonObject colorObj = doc["color"];
  if (!colorObj.containsKey("r") || !colorObj.containsKey("g") || 
      !colorObj.containsKey("b") || !colorObj.containsKey("w")) {
    Serial.println("Missing color components");
    DynamicJsonDocument errorDoc(128);
    errorDoc["cmd"] = "error";
    errorDoc["message"] = "Missing color components";
    String errorResponse;
    serializeJson(errorDoc, errorResponse);
    broadcastMessage(errorResponse.c_str());
    return;
  }
  
  Color color;
  color.r = colorObj["r"];
  color.g = colorObj["g"];
  color.b = colorObj["b"];
  color.w = colorObj["w"];
  color.brightness = doc.containsKey("brightness") ? doc["brightness"] : 100;
  
  // 验证颜色值范围
  if (color.r > 255 || color.g > 255 || color.b > 255 || color.w > 255 || color.brightness > 100) {
    Serial.println("Invalid color values");
    DynamicJsonDocument errorDoc(128);
    errorDoc["cmd"] = "error";
    errorDoc["message"] = "Invalid color values";
    String errorResponse;
    serializeJson(errorDoc, errorResponse);
    broadcastMessage(errorResponse.c_str());
    return;
  }
  
  setStaticColor(color);
  
  // 发送确认消息
  String response;
  doc["status"] = "ok";
  serializeJson(doc, response);
  broadcastMessage(response.c_str());
}

void parseEffectCommand(const char* json) {
  DynamicJsonDocument doc(256);
  DeserializationError error = deserializeJson(doc, json);
  
  if (error) {
    Serial.printf("JSON parsing error: %s\n", error.c_str());
    return;
  }
  
  // 检查必要字段
  if (!doc.containsKey("name")) {
    Serial.println("Missing 'name' field");
    DynamicJsonDocument errorDoc(128);
    errorDoc["cmd"] = "error";
    errorDoc["message"] = "Missing 'name' field";
    String errorResponse;
    serializeJson(errorDoc, errorResponse);
    broadcastMessage(errorResponse.c_str());
    return;
  }
  
  if (!doc.containsKey("color")) {
    Serial.println("Missing 'color' field");
    DynamicJsonDocument errorDoc(128);
    errorDoc["cmd"] = "error";
    errorDoc["message"] = "Missing 'color' field";
    String errorResponse;
    serializeJson(errorDoc, errorResponse);
    broadcastMessage(errorResponse.c_str());
    return;
  }
  
  const char* effectName = doc["name"];
  uint8_t speed = doc.containsKey("speed") ? doc["speed"] : 50;
  
  JsonObject colorObj = doc["color"];
  if (!colorObj.containsKey("r") || !colorObj.containsKey("g") || 
      !colorObj.containsKey("b") || !colorObj.containsKey("w")) {
    Serial.println("Missing color components");
    DynamicJsonDocument errorDoc(128);
    errorDoc["cmd"] = "error";
    errorDoc["message"] = "Missing color components";
    String errorResponse;
    serializeJson(errorDoc, errorResponse);
    broadcastMessage(errorResponse.c_str());
    return;
  }
  
  Color color;
  color.r = colorObj["r"];
  color.g = colorObj["g"];
  color.b = colorObj["b"];
  color.w = colorObj["w"];
  color.brightness = doc.containsKey("brightness") ? doc["brightness"] : 100;
  
  // 验证参数范围
  if (speed > 100 || color.r > 255 || color.g > 255 || 
      color.b > 255 || color.w > 255 || color.brightness > 100) {
    Serial.println("Invalid parameter values");
    DynamicJsonDocument errorDoc(128);
    errorDoc["cmd"] = "error";
    errorDoc["message"] = "Invalid parameter values";
    String errorResponse;
    serializeJson(errorDoc, errorResponse);
    broadcastMessage(errorResponse.c_str());
    return;
  }
  
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
  DeserializationError error = deserializeJson(doc, json);
  
  if (error) {
    Serial.printf("JSON parsing error: %s\n", error.c_str());
    return;
  }
  
  // 检查必要字段
  if (!doc.containsKey("on")) {
    Serial.println("Missing 'on' field");
    DynamicJsonDocument errorDoc(128);
    errorDoc["cmd"] = "error";
    errorDoc["message"] = "Missing 'on' field";
    String errorResponse;
    serializeJson(errorDoc, errorResponse);
    broadcastMessage(errorResponse.c_str());
    return;
  }
  
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
