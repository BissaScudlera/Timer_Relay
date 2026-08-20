#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#ifdef ARDUINO_ARCH_ESP32

#include <WiFi.h>
#include <WebServer.h>

#include "Globals.h"
#include "RTCManager.h"
#include "Debug.h"

#include "WebPages.h"
#include "WebHandlers.h"

#include <Preferences.h>
/*
#if DEBUG
 #define AP_SSID "ESP32"
 #define AP_PWD  "12345678"
#else
 #include "wifi_config.h"
#endif
*/ #include "wifi_config.h"

//---------------------------------------------------------------------------
// Oggetti globali del WebServer
//---------------------------------------------------------------------------

extern WebServer server;

//extern const char* Wifi_ssid;
//extern const char* Wifi_password;
struct ConfigW
{
  char ssid[33];
  char pswd[65];
};

extern ConfigW configWifi;


extern bool webServerAttivo;

//---------------------------------------------------------------------------
// API pubblica
//---------------------------------------------------------------------------
void turnOffWebServer();
void turnOnWebServer();
void toggleWebServer();

void serverSetup();

void serverLoop();


bool saveConfigWifi();
bool loadConfigWifi();


#endif

#endif