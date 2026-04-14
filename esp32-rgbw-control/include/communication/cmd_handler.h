#ifndef CMD_HANDLER_H
#define CMD_HANDLER_H

#include <stdint.h>

void handleCommand(char* command);
void handleBinaryCommand(uint8_t* data);
void handleProtocolNegotiation();
void parseStaticCommand(const char* json);
void parseEffectCommand(const char* json);
void parseSwitchCommand(const char* json);
void parseGetStatusCommand();

#endif
