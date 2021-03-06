/* ************************************************************************ */
/*
    esp8266-ino.h - support for the associated ino file.
*/
#ifndef ESP8266INO_H
#define ESP8266INO_H

#include "AppCfgData.h"
#include "WifiCfgData.h"
#include "SrvCfgData.h"
#include "connectWiFi.h"

// support functions - 
extern void setupStart();
extern void setupDone();
extern void toggleLED();
extern bool setupApp(const String appCfgFile);
extern bool setupWiFi(const String wifiCfgFile);
extern bool setupServers(const String srvCfgFile);

// pointers to configuration data objects - 
extern AppCfgData *a_cfgdat;
extern WifiCfgData *w_cfgdat;
extern SrvCfgData *s_cfgdat;

// pointer to the WiFi connection object -
extern ConnectWiFi *connWiFi;

#endif

