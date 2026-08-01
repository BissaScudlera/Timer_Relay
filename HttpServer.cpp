#include "HttpServer.h"

#ifdef ARDUINO_ARCH_ESP32

//------------------------------------------------------------
// Variabili globali
//------------------------------------------------------------

//Wifi Configuration
const char* ssid = "ESP32" ;
const char* password = "12345678" ;

WebServer server(80);

bool webServerAttivo = true;

//------------------------------------------------------------


void toggleWebServer()
{
    if (webServerAttivo)
    {
        server.stop();

        WiFi.softAPdisconnect(true);

        webServerAttivo = false;

        DBG_PRINT("\n[SISTEMA] Wi-Fi e Web Server DISATTIVATI.\n");
    }
    else
    {
        if (WiFi.softAP(ssid, password))
        {
            server.begin();

            webServerAttivo = true;

            DBG_PRINT("\n[SISTEMA] Wi-Fi e Web Server ATTIVATI.\n");
            DBG_PRINT("[SISTEMA] SSID: " + String(ssid) + "\n");
            DBG_PRINT("[SISTEMA] IP: 192.168.4.1\n");
        }
    }
}

//------------------------------------------------------------

void serverSetup()
{
    if (strlen(password) < 8)
    {
        DBG_PRINT("\n[ERRORE WI-FI] Password troppo corta.\n");
        return;
    }

    if (WiFi.softAP(ssid, password))
    {
        DBG_PRINT("\n[WI-FI] Access Point creato.\n");
        DBG_PRINT("[WI-FI] SSID: " + String(ssid) + "\n");
        DBG_PRINT("[WI-FI] IP: 192.168.4.1\n");

        server.on("/",              gestisciRoot);
        server.on("/comandi",       gestisciComandi);
        server.on("/config",        gestisciConfigurazione);
        server.on("/sistema",       gestisciSistema);
        server.on("/setupRTC",      gestisciSetupRTC);
        server.on("/setupWiFi",     gestisciSetupWiFi);

        server.on("/start", HTTP_POST, gestisciStart);
        server.on("/stop", HTTP_POST, gestisciStop);

        server.on("/salva-cfg", HTTP_POST, gestisciSalvaCfg);
        server.on("/salva-rtc", HTTP_POST, gestisciSalvaRTC);
        server.on("/salva-wifi", HTTP_POST, gestisciSalvaWiFi);
        server.on("/salva-wifi-eeprom", HTTP_POST, gestisciSalvaWiFiEEPROM);

        server.begin();

        delay(50);
    }
}

//------------------------------------------------------------

void serverLoop()
{
    if (webServerAttivo)
    {
        server.handleClient();
    }
}

#endif