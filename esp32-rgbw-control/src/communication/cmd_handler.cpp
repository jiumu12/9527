#include "communication/cmd_handler.h"
#include "config/config.h"
#include "light/light_engine.h"
#include "communication/com_server.h"
#include <ArduinoJson.h>

void handleCommand(char* command) {
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
  } else {
    Serial.printf("Unknown command: %s\n", cmd);
  }
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
