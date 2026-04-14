#include "com_server.h"
#include "config.h"
#include "cmd_handler.h"
#include <Arduino.h>

WebSocketsServer webSocket = WebSocketsServer(SERVER_PORT);

void initWebSocketServer() {
  webSocket.begin();
  webSocket.onEvent(handleWebSocketEvent);
  Serial.println("WebSocket server started");
}

void handleWebSocketEvent(uint8_t client_num, WStype_t type, uint8_t *payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
      Serial.printf("Client %u disconnected\n", client_num);
      break;
    case WStype_CONNECTED:
      Serial.printf("Client %u connected\n", client_num);
      break;
    case WStype_TEXT:
      Serial.printf("Received message: %s\n", payload);
      // 处理心跳包
      if (strstr((char*)payload, "ping") != NULL) {
        webSocket.sendTXT(client_num, "{\"cmd\": \"pong\"}");
      } else {
        handleCommand((char*)payload);
      }
      break;
    case WStype_ERROR:
      Serial.printf("WebSocket error\n");
      break;
  }
}

void broadcastMessage(const char* message) {
  webSocket.broadcastTXT(message);
  Serial.printf("Broadcasted message: %s\n", message);
}
