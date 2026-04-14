#ifndef CMD_HANDLER_H
#define CMD_HANDLER_H

#include <stdint.h>

void handleCommand(char* command);
void handleBinaryCommand(uint8_t* data);
void handleProtocolNegotiation(int msgId);
void parseStaticCommand(const char* json, int msgId);
void parseEffectCommand(const char* json, int msgId);
void parseSwitchCommand(const char* json, int msgId);
void parseGetStatusCommand(int msgId);

#endif
