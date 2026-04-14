#ifndef COM_SERVER_H
#define COM_SERVER_H

#include <WebSocketsServer.h>
#include <WebServer.h>

extern WebSocketsServer webSocket;
extern WebServer server;

void initWebSocketServer();
void handleWebSocketEvent(uint8_t client_num, WStype_t type, uint8_t *payload, size_t length);
void broadcastMessage(const char* message);

#endif
