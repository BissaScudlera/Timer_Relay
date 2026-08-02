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
#include "wifi_config.h"

//---------------------------------------------------------------------------
// Oggetti globali del WebServer
//---------------------------------------------------------------------------

extern WebServer server;

extern const char* Wifi_ssid;
extern const char* Wifi_password;

extern bool webServerAttivo;

//---------------------------------------------------------------------------
// API pubblica
//---------------------------------------------------------------------------

void toggleWebServer();

void serverSetup();

void serverLoop();

#endif

#endif