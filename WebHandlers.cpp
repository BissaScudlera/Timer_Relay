#include "WebHandlers.h"

#ifdef ARDUINO_ARCH_ESP32

#include <Arduino.h>
#include "Globals.h"
#include "RTCManager.h"
#include "RelayManager.h"
#include "HttpServer.h"

void gestisciStart()
{
    relayStart();
    server.sendHeader("Location", "/comandi");
    server.send(303);
}

void gestisciStop()
{
    relayStop();
    server.sendHeader("Location", "/comandi");
    server.send(303);
}

void readTimerConfigFromRequest(){
	for (int i = 0; i < RELAY_NUMBER; i++)
    {
        configTimer.relayEnableMask[i] = server.hasArg("r" + String(i));
    }

    for (int i = 0; i < 7; i++)
    {
        configTimer.dayEnableMask[i] = server.hasArg("d" + String(i));
    }

    if (server.hasArg("durata"))
    {
        unsigned long d = server.arg("durata").toInt();
        if (d > 0) configTimer.relayDuration = d;
    }

    if (server.hasArg("orario"))
    {
        String t = server.arg("orario");
        configTimer.startHour = t.substring(0,2).toInt();
        configTimer.startMinute = t.substring(3,5).toInt();
        configTimer.startSecond = 0;
    }
}

void gestisciSalvaCfg()
{
    readTimerConfigFromRequest();
    server.sendHeader("Location", "/config?saved=1");
    server.send(303);
}

void gestisciSalvaCfgEEPROM()
{
	readTimerConfigFromRequest();
	saveConfigTimer();
	DBG_PRINTLN("[CFG] Saved timer config to EEPROM");
    server.sendHeader("Location", "/config?saved=1");
    server.send(303);
}

void gestisciSalvaRTC()
{
    if (server.hasArg("data") && server.hasArg("ora"))
    {
        String d = server.arg("data");
        String o = server.arg("ora");

        int giorno = d.substring(8,10).toInt();
        int mese = d.substring(5,7).toInt();
        int anno = d.substring(0,4).toInt();
        int ora = o.substring(0,2).toInt();
        int minuto = o.substring(3,5).toInt();
        int secondo = o.substring(6,8).toInt();

        rtcSetDateTime(DateTime(anno,mese,giorno,ora,minuto,secondo));
    }

    server.sendHeader("Location", "/");
    server.send(303);
}

void readWifiConfigFromRequest(){
	if(server.hasArg("ssid") && server.hasArg("password"))
    {
        String nuovoSSID = server.arg("ssid");
        String nuovaPassword = server.arg("password");

        if(nuovoSSID.length() > 0 && nuovaPassword.length() >= 8)
        {
            strlcpy(configWifi.ssid, nuovoSSID.c_str(), sizeof(configWifi.ssid));
            strlcpy(configWifi.pswd, nuovaPassword.c_str(), sizeof(configWifi.pswd));

            //Wifi_ssid = configWifi.ssid;
            //Wifi_password = configWifi.pswd;

            WiFi.softAPdisconnect(true);
            delay(100);
            WiFi.softAP(configWifi.ssid, configWifi.pswd);
        }
    }
}

void gestisciSalvaWiFi()
{
    readWifiConfigFromRequest();
    server.sendHeader("Location", "/setupWiFi?saved=1");
    server.send(303);
}


void gestisciSalvaWiFiEEPROM()
{
    readWifiConfigFromRequest();
	// Salvataggio permanente configurazione WiFi su NVS.
	saveConfigWifi();
	DBG_PRINTLN("[CFG] Saved Wifi config to EEPROM");
    server.sendHeader("Location", "/setupWiFi?saved=1");
    server.send(303);
}

#endif
