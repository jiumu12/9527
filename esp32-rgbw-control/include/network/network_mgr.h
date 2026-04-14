#ifndef NETWORK_MGR_H
#define NETWORK_MGR_H

#include <WiFi.h>
#include <ESPmDNS.h>
#include "config/config.h"

void initNetwork();
void connectWiFi();
void setupMDNS();
void checkNetworkStatus();
Config* getGlobalConfig();
void updateConfig(Config* newConfig);

#endif
