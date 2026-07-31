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

//---------------------------------------------------------------------------
// Oggetti globali del WebServer
//---------------------------------------------------------------------------

extern WebServer server;

extern const char* ssid;
extern const char* password;

extern bool webServerAttivo;

//---------------------------------------------------------------------------
// API pubblica
//---------------------------------------------------------------------------

void toggleWebServer();

void serverSetup();

void serverLoop();

#endif

#endif