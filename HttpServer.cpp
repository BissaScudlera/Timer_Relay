#include "HttpServer.h"

#ifdef ARDUINO_ARCH_ESP32

//------------------------------------------------------------
// Variabili globali
//------------------------------------------------------------


//Wifi Configuration

//const char* Wifi_ssid = AP_SSID ;     //configWifi.ssid
//const char* Wifi_password = AP_PWD ;  //configWifi.pswd

ConfigW configWifi =
{
	AP_SSID,
	AP_PWD
};


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
        if (WiFi.softAP(configWifi.ssid, configWifi.pswd))
        {
            server.begin();

            webServerAttivo = true;

            DBG_PRINTLN("\r\n[SYS] Wi-Fi e Web Server ATTIVATI.");
            DBG_PRINT("[SYS] SSID: ");
            DBG_PRINTLN(String(configWifi.ssid));
            DBG_PRINTLN("[SYS] IP: 192.168.4.1");
        }
    }
}

//------------------------------------------------------------

void serverSetup()
{
    /* WiFi configuration initialized by setup()
	//Load saved WiFi AP credentials
    if (!loadConfigWifi() || FactoryReset){
	  DBG_PRINTLN("[CFG] Restoring default Wifi settings");
	  //saveConfigWifi();
	  strlcpy(configWifi.ssid, AP_SSID, sizeof(configWifi.ssid));
      strlcpy(configWifi.pswd, AP_PWD, sizeof(configWifi.pswd));
    }*/
	
    if (strlen(configWifi.pswd) < 8)
    {
        DBG_PRINTLN("[WI-FI] Errore! Password troppo corta.");
        return;
    }

    if (WiFi.softAP(configWifi.ssid, configWifi.pswd))
    {
        DBG_PRINTLN("[WI-FI] Access Point creato.");
        DBG_PRINTLN("[WI-FI] SSID: " + String(configWifi.ssid));
        DBG_PRINTLN("[WI-FI] IP: 192.168.4.1");

        server.on("/",              gestisciRoot);
        server.on("/comandi",       gestisciComandi);
        server.on("/config",        gestisciConfigurazione);
        server.on("/sistema",       gestisciSistema);
        server.on("/setupRTC",      gestisciSetupRTC);
        server.on("/setupWiFi",     gestisciSetupWiFi);
		
		server.on("/help", HTTP_GET, gestisciHelp);

        server.on("/start", HTTP_POST, gestisciStart);
        server.on("/stop", HTTP_POST, gestisciStop);

        server.on("/salva-cfg", HTTP_POST, gestisciSalvaCfg);
        server.on("/salva-cfg-eeprom", HTTP_POST, gestisciSalvaCfgEEPROM);
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


bool saveConfigWifi(){
	Preferences prefs;
	
	//strlcpy(configWifi.ssid, Wifi_ssid, sizeof(configWifi.ssid));
    //strlcpy(configWifi.pswd, Wifi_password, sizeof(configWifi.pswd));
	
	prefs.begin("wifi", false);

    size_t written = prefs.putBytes("config", &configWifi, sizeof(configWifi));

    prefs.end();

    return (written == sizeof(configWifi));
}

bool loadConfigWifi(){
    Preferences prefs;
    	
	prefs.begin("wifi", true);   // sola lettura

    if (prefs.getBytesLength("config") != sizeof(configWifi))
    {
        prefs.end();
	    DBG_PRINTLN("[CFG] Error reading Wifi settings");
        return false;             // nessuna configurazione valida
    }

    prefs.getBytes("config", &configWifi, sizeof(configWifi));

    prefs.end();

    return true;
}


#endif