#include "WebPages.h"

#ifdef ARDUINO_ARCH_ESP32

#include <Arduino.h>
#include "Globals.h"
#include "ConfigManager.h"
#include "RTCManager.h"
#include "RelayManager.h"
#include "Diagnostics.h"
#include "HttpServer.h"

static String paginaInizio(const String& titolo)
{
    String html;
    html += "<!DOCTYPE html><html><head>";
    html += "<meta name='viewport' content='width=device-width,initial-scale=1'>";
    html += "<title>" + titolo + "</title>";
    html += "<style>";
    html += "body{font-family:Arial;text-align:center;background:#f2f2f2}";
    html += ".card{background:white;margin:10px auto;padding:15px;border-radius:10px;max-width:500px}";
    html += "button{padding:12px;margin:5px}";
    html += "table{margin:auto;border-collapse:collapse}";
    html += "td,th{border:1px solid #ccc;padding:8px}";
    html += ".toggle{width:20px;height:20px}";
    html += "</style></head><body>";

    html += "<a href='/'><button>Home</button></a>";
    html += "<a href='/comandi'><button>Comandi</button></a>";
    html += "<a href='/config'><button>Configurazione</button></a>";
    html += "<a href='/sistema'><button>Sistema</button></a>";

    return html;
}

static String listaUsciteAttive()
{
    String s;

    bool presente = false;

    for(int i=0;i<RELAY_NUMBER;i++)
    {
        if(relay[i])
        {
            s += "Valvola ";
            s += String(i+1);
            s += "<br>";
            presente = true;
        }
    }

    if(!presente)
        s = "Nessuna";

    return s;
}

static String tabellaRelay()
{
    String s;
    s += "<table>";

    for(int i=0;i<RELAY_NUMBER;i++)
    {
        if(i % 4 == 0)
            s += "<tr>";

        s += "<td>";
        s += "Val ";
        s += String(i+1);
        s += "<br>";

        s += "<input class='toggle' type='checkbox' name='r";
        s += String(i);
        s += "'";
        if(config.relayEnableMask[i])
            s += " checked";
        s += ">";

        s += "</td>";

        if(i % 4 == 3)
            s += "</tr>";
    }

    if(RELAY_NUMBER % 4 != 0)
        s += "</tr>";

    s += "</table>";

    return s;
}

static String giorniAbilitati()
{
    const char* giorni[7] =
    {
        "Lun","Mar","Mer","Gio","Ven","Sab","Dom"
    };

    String s;

    for(int i=0;i<7;i++)
    {
        if(config.dayEnableMask[i])
        {
            if(s.length())
                s += " - ";

            s += giorni[i];
        }
    }

    if(s.length()==0)
        s="Nessuno";

    return s;
}


void gestisciRoot()
{
    String html = paginaInizio("Home");
    html += "<meta http-equiv='refresh' content='1'>";

    html += "<div class='card'><h2>Stato irrigazione</h2>";
    html += relayRunning() ? "ATTIVA" : "FERMA";
    html += "</div>";

    html += "<div class='card'><h2>Data / Giorno / Ora</h2>";
    if(rtcGetStatus().available)
    {
        html += rtcDateString();
        html += "<br>";
        html += rtcTimeString();
    }
    else{
        html += "--/--/----";
        html += "<br>";
        html += "--:--:--";
    }
    html += "</div>";

    html += "<div class='card'><h2>Programma</h2>";
    html += "Ora avvio: ";
    if(config.startHour < 10) html += "0";
    html += String(config.startHour);
    html += ":";
    if(config.startMinute < 10) html += "0";
    html += String(config.startMinute);
    html += "<br>Durata step: ";
    html += String(config.relayDuration);
    html += " sec";
    html += "<br>Giorni: ";
    html += giorniAbilitati();
    html += "</div>";

    html += "<div class='card'><h2>Uscite attive</h2>";
    html += listaUsciteAttive();
    html += "<br>Tempo rimanente step: ";
    html += String(relayRemainingSeconds());
    html += " sec</div>";

    html += "</body></html>";

    server.send(200,"text/html",html);
}


void gestisciComandi()
{
    String html = paginaInizio("Comandi");
    html += "<meta http-equiv='refresh' content='1'>";

    html += "<div class='card'>";
    html += "<h2>Comandi manuali</h2>";

    html += "<form action='/start' method='POST'><button>AVVIA CICLO</button></form>";
    html += "<form action='/stop' method='POST'><button>STOP EMERGENZA</button></form>";

    html += "</div>";

    html += "<div class='card'><h2>Stato irrigazione</h2>";
    html += relayRunning() ? "ATTIVA" : "FERMA";
    html += "</div>";

    html += "<div class='card'><h2>Uscite attive</h2>";
    html += listaUsciteAttive();
    html += "</div>";

    html += "<div class='card'><h2>Tempo</h2>";
    html += String(relayRemainingSeconds());
    html += " sec / ";
    html += String(config.relayDuration);
    html += " sec</div>";

    html += "</body></html>";

    server.send(200,"text/html",html);
}



static String giorniEditabili()
{
    const char* d[]={"Lun","Mar","Mer","Gio","Ven","Sab","Dom"};
    String h="<table><tr>";
    for(int i=0;i<7;i++){ h+="<td>"; h+=d[i]; h+="</td>";}
    h+="</tr><tr>";
    for(int i=0;i<7;i++){
        h+="<td><input type='checkbox' name='d";
        h+=String(i);
        h+="'";
        if(config.dayEnableMask[i]) h+=" checked";
        h+="></td>";
    }
    h+="</tr></table>";
    return h;
}

void gestisciConfigurazione()
{
    String html = paginaInizio("Configurazione");

    html += "<div class='card'>";
    html += "<form action='/salva-maschere' method='POST'>";
    html += "<button>SALVA</button>";

    html += "<h3>Orario partenza</h3>";
    html += "<input type='time' name='orario' value='";
    if(config.startHour < 10) html += "0";
    html += String(config.startHour);
    html += ":";
    if(config.startMinute < 10) html += "0";
    html += String(config.startMinute);
    html += "'>";

    html += "<h3>Durata step [s]</h3>";
    html += "<input name='durata' value='";
    html += String(config.relayDuration);
    html += "'>";

    html += "<h3>Giorni abilitati</h3>";
    html += giorniEditabili();

    html += "<h3>Valvole abilitate</h3>";
    html += tabellaRelay();

    html += "</form></div>";

    html += "</body></html>";

    server.send(200,"text/html",html);
}


void gestisciSistema()
{
    String html = paginaInizio("Sistema");
    html += "<meta http-equiv='refresh' content='1'>";

    html += "<div class='card'>";
    html += "<h2>Diagnostica</h2>";
    html += "I2C: ";
    html += diagnosticsI2CState();
    html += "<br>RTC: ";
    html += diagnosticsRtcState();
    html += "<br>WiFi: ";
    html += diagnosticsWiFiState();
    html += "<br>Client connessi: ";
    html += String(WiFi.softAPgetStationNum());
    html += "<br><br>Uptime: ";
    unsigned long up = millis() / 1000;
    html += String(up / 3600);
    html += "h ";
    html += String((up % 3600) / 60);
    html += "m ";
    html += String(up % 60);
    html += "s";
    html += "<br>Heap libero: ";
    html += String(ESP.getFreeHeap() / 1024);
    html += " KB";
    html += "<br>Tcpu: ";
    html += String(temperatureRead(), 1);
    html += " C";
    html += "<br>Trtc: ";
    float trtc = rtcTemperature();
    if (isnan(trtc))
        html += "--";
    else
        html += String(trtc,1);
    html += " C";

    html += "<hr>";
    html += "<a href='/setupRTC'><button>SETUP RTC</button></a>";
    html += "<br><a href='/setupWiFi'><button>SETUP WIFI</button></a>";

    html += "</div>";

    html += "</body></html>";

    server.send(200,"text/html",html);
}


void gestisciSetupRTC()
{
    String html = paginaInizio("Setup RTC");

    html += "<div class='card'>";
    html += "<h2>Setup RTC</h2>";

    html += "<form method='POST' action='/salva-rtc'>";
    html += "Data:<br>";
    html += "<input type='date' name='data'><br>";

    html += "Ora:<br>";
    html += "<input type='time' name='ora' step='1'><br><br>";

    html += "<button type='submit'>SALVA RTC</button>";
    html += "</form>";

    html += "</div>";

    html += "</body></html>";

    server.send(200,"text/html",html);
}


void gestisciSetupWiFi()
{
    String html = paginaInizio("Setup WiFi");

    html += "<div class='card'>";
    html += "<h2>Setup WiFi</h2>";

    html += "<form method='POST' action='/salva-wifi'>";

    html += "SSID:<br>";
    html += "<input type='text' name='ssid' value='";
    html += ssid;
    html += "'><br>";

    html += "Password:<br>";
    html += "<input type='password' name='password'><br>";

    html += "IP AP attuale:<br>";
    html += WiFi.softAPIP().toString();
    html += "<br><br>";

    html += "Netmask:<br>";
    html += WiFi.softAPSubnetMask().toString();
    html += "<br><br>";

    html += "<button type='submit'>SALVA WIFI</button>";
    html += "<br><button disabled>SALVA SU EEPROM</button>";

    html += "</form>";
    html += "</div>";

    html += "</body></html>";

    server.send(200,"text/html",html);
}

#endif
