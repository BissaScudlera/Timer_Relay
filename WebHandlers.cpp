#include "WebHandlers.h"

#ifdef ARDUINO_ARCH_ESP32

#include <Arduino.h>

#include "Globals.h"
#include "RTCManager.h"
#include "RelayManager.h"
#include "WebServer.h"

void gestisciStart()
{
    relayStart();

    server.sendHeader("Location", "/setup");
    server.send(303);
}

void gestisciStop()
{
    relayStop();

    server.sendHeader("Location", "/setup");
    server.send(303);
}

void gestisciSalvaDurata()
{
    if (server.hasArg("durata"))
    {
        long nuovaDurata = server.arg("durata").toInt();

        if (nuovaDurata > 0)
        {
            config.relayDuration = nuovaDurata;
        }
    }

    server.sendHeader("Location", "/setup");
    server.send(303);
}

void gestisciSalvaOra()
{
    if (rtcGetStatus().available && server.hasArg("orario"))
    {
        String t = server.arg("orario");

        int h = t.substring(0, 2).toInt();
        int m = t.substring(3, 5).toInt();

        rtcSetTime(h, m);
    }

    server.sendHeader("Location", "/setup");
    server.send(303);
}

void gestisciSalvaMaschere()
{
    // Relè
    for (int i = 0; i < RELAY_NUMBER; i++)
    {
        String nome = "r" + String(i);
        config.relayEnableMask[i] = server.hasArg(nome);
    }

    // Giorni
    for (int i = 0; i < 7; i++)
    {
        String nome = "d" + String(i);
        config.dayEnableMask[i] = server.hasArg(nome);
    }

    server.sendHeader("Location", "/setup");
    server.send(303);
}
#endif