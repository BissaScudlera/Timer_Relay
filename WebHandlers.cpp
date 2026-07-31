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

void gestisciSalvaMaschere()
{
    for (int i = 0; i < RELAY_NUMBER; i++)
    {
        config.relayEnableMask[i] = server.hasArg("r" + String(i));
    }

    for (int i = 0; i < 7; i++)
    {
        config.dayEnableMask[i] = server.hasArg("d" + String(i));
    }

    if (server.hasArg("durata"))
    {
        unsigned long d = server.arg("durata").toInt();
        if (d > 0) config.relayDuration = d;
    }

    if (server.hasArg("orario"))
    {
        String t = server.arg("orario");
        config.startHour = t.substring(0,2).toInt();
        config.startMinute = t.substring(3,5).toInt();
        config.startSecond = 0;
    }

    server.sendHeader("Location", "/");
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


void gestisciSalvaWiFi()
{
    if(server.hasArg("ssid") && server.hasArg("password"))
    {
        String nuovoSSID = server.arg("ssid");
        String nuovaPassword = server.arg("password");

        if(nuovoSSID.length() > 0 && nuovaPassword.length() >= 8)
        {
            ssid = strdup(nuovoSSID.c_str());
            password = strdup(nuovaPassword.c_str());

            WiFi.softAPdisconnect(true);
            delay(100);
            WiFi.softAP(ssid, password);
        }
    }

    server.sendHeader("Location", "/");
    server.send(303);
}


void gestisciSalvaWiFiEEPROM()
{
    // FUTURO:
    // Salvataggio permanente configurazione WiFi su EEPROM/NVS.
    // Parametri previsti:
    // - SSID
    // - password
    // - configurazione rete AP
    //
    // Attualmente la configurazione WiFi resta solo in memoria volatile.
}

#endif
