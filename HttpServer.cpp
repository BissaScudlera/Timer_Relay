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

        DBG_PRINTLN("\r\n[SYS] Wi-Fi e Web Server DISATTIVATI.");
    }
    else
    {
        if (WiFi.softAP(ssid, password))
        {
            server.begin();

            webServerAttivo = true;

            DBG_PRINTLN("\r\n[SYS] Wi-Fi e Web Server ATTIVATI.");
            DBG_PRINTLN("[SYS] SSID: " + String(ssid) );
            DBG_PRINTLN("[SYS] IP: 192.168.4.1");
        }
    }
}

//------------------------------------------------------------

void serverSetup()
{
    if (strlen(password) < 8)
    {
        DBG_PRINTLN("[WI-FI] Errore! Password troppo corta.");
        return;
    }

    if (WiFi.softAP(ssid, password))
    {
        DBG_PRINTLN("[WI-FI] Access Point creato.");
        DBG_PRINTLN("[WI-FI] SSID: " + String(ssid));
        DBG_PRINTLN("[WI-FI] IP: 192.168.4.1");

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