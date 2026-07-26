#include "WebServer.h"

#ifdef ARDUINO_ARCH_ESP32

//------------------------------------------------------------
// Variabili globali
//------------------------------------------------------------

const char* ssid = "irrigazione";
const char* password = "michelone";

WebServer server(80);

bool webServerAttivo = true;

//------------------------------------------------------------
// Riferimenti esterni
//------------------------------------------------------------

extern String generaHtmlRele();
extern String generaHtmlGiorni();

extern int ErrState;

extern const int iResetButton;

extern unsigned long t_iReset;
extern bool lastResetStatus;

extern const unsigned long T_iResetLong;
extern const unsigned long T_iResetShort;

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
        server.on("/setup",         gestisciSetup);

        server.on("/start", HTTP_POST, gestisciStart);
        server.on("/stop", HTTP_POST, gestisciStop);

        server.on("/salva-durata", HTTP_POST, gestisciSalvaDurata);
        server.on("/salva-ora", HTTP_POST, gestisciSalvaOra);
        server.on("/salva-maschere", HTTP_POST, gestisciSalvaMaschere);

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