#ifndef CMD_HANDLER_H
#define CMD_HANDLER_H

void handleCommand(char* command);
void parseStaticCommand(const char* json);
void parseEffectCommand(const char* json);
void parseSwitchCommand(const char* json);
void parseGetStatusCommand();

#endif
