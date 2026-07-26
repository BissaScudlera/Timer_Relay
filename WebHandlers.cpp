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
    /*
        Qui verrà implementato il ConfigManager.

        Per ora manteniamo esattamente il comportamento
        originale senza modificare la logica.
    */

    for (int i = 0; i < RELAY_NUMBER; i++)
    {
        // placeholder
    }

    server.sendHeader("Location", "/setup");
    server.send(303);
}

#endif