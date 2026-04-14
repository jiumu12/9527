#ifndef NETWORK_MGR_H
#define NETWORK_MGR_H

#include <WiFi.h>
#include <ESPmDNS.h>

void initNetwork();
void connectWiFi();
void setupMDNS();
void checkNetworkStatus();

#endif
