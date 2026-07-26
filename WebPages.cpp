#include "WebPages.h"

#ifdef ARDUINO_ARCH_ESP32

#include <Arduino.h>

#include "Globals.h"
#include "RTCManager.h"
#include "RelayManager.h"
#include "WebServer.h"

// Funzioni generate nello sketch principale
extern String generaHtmlRele();
extern String generaHtmlGiorni();

void gestisciRoot()
{
    String html = "<!DOCTYPE html><html><head><meta charset='UTF-8'>";
    html += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
    html += "<meta http-equiv='refresh' content='2'>";
    html += "<title>Monitoraggio Irrigazione</title>";

    html += "<style>";
    html += "body{font-family:sans-serif;text-align:center;padding:20px;background:#f4f4f9;color:#333;}";
    html += ".card{background:white;padding:25px;border-radius:12px;max-width:450px;margin:20px auto;box-shadow:0 4px 15px rgba(0,0,0,0.08);}";
    html += "button{font-size:16px;font-weight:bold;padding:12px 24px;margin-top:20px;border:none;border-radius:6px;cursor:pointer;width:80%;background:#007BFF;color:white;}";
    html += ".stato{font-weight:bold;font-size:22px;margin:15px 0;}";
    html += ".attivo{color:#28a745;}";
    html += ".spento{color:#6c757d;}";
    html += ".info-table{width:100%;margin-top:15px;border-collapse:collapse;}";
    html += ".info-table td{padding:8px;border-bottom:1px solid #eee;text-align:left;}";
    html += ".info-table td:last-child{text-align:right;font-weight:bold;}";
    html += "</style></head><body>";

    html += "<div class='card'><h2>📊 Monitoraggio Rete</h2>";

    if(relayRunning())
        html += "<p class='stato attivo'>● IRRIGAZIONE IN CORSO</p>";
    else
        html += "<p class='stato spento'>○ SISTEMA IN ATTESA</p>";

    html += "<table class='info-table'>";

    if(rtcGetStatus().available)
    {
        html += "<tr><td>Orario RTC:</td><td>";
        html += rtcTimeString();
        html += "</td></tr>";
    }

    html += "<tr><td>Durata Canale:</td><td>";
    html += String(config.relayDuration);
    html += " s</td></tr>";

    if(relayRunning())
    {
        unsigned long rimasti =
            (config.relayDuration > relayActiveSeconds) ?
            config.relayDuration - relayActiveSeconds : 0;

        html += "<tr><td>Valvola Attiva:</td><td>";
        html += String(currentRelayIndex + 1);
        html += " / ";
        html += String(RELAY_NUMBER);
        html += "</td></tr>";

        html += "<tr><td>Tempo Rimasto Step:</td><td>";
        html += String(rimasti);
        html += " s</td></tr>";
    }
    else
    {
        html += "<tr><td>Valvola Attiva:</td><td>Nessuna</td></tr>";
    }

    html += "</table>";

    html += "<a href='/setup'>";
    html += "<button>⚙️ Configura Parametri</button>";
    html += "</a>";

    html += "</div></body></html>";

    server.send(200,"text/html",html);
}

void gestisciSetup()
{
    String html = "<!DOCTYPE html><html><head><meta charset='UTF-8'>";
    html += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
    html += "<title>Configurazione</title>";

    html += "<style>";
    html += "body{font-family:sans-serif;text-align:center;padding:20px;background:#e9ecef;color:#333;}";
    html += ".card{background:white;padding:25px;border-radius:12px;max-width:450px;margin:20px auto;box-shadow:0 4px 15px rgba(0,0,0,.1);text-align:left;}";
    html += "h2,h3{text-align:center;}";
    html += "button{font-size:16px;font-weight:bold;padding:10px 20px;margin:10px auto;border:none;border-radius:6px;cursor:pointer;width:100%;display:block;}";
    html += ".btn-start{background:#28a745;color:white;}";
    html += ".btn-stop{background:#dc3545;color:white;}";
    html += ".btn-back{background:#6c757d;color:white;}";
    html += ".btn-save{background:#17a2b8;color:white;}";
    html += "input[type='number'],input[type='time']{width:93%;padding:8px;font-size:16px;margin:5px 0 15px;border:1px solid #ccc;border-radius:4px;}";
    html += ".sezione{margin-top:20px;padding-top:15px;border-top:1px solid #eee;}";
    html += ".grid-toggle{display:grid;grid-template-columns:repeat(4,1fr);gap:10px;margin:15px 0;}";
    html += ".grid-days{display:grid;grid-template-columns:repeat(4,1fr);gap:10px;margin:15px 0;}";
    html += ".toggle-container{display:flex;flex-direction:column;align-items:center;font-size:12px;font-weight:bold;}";
    html += ".switch{position:relative;display:inline-block;width:45px;height:24px;margin-bottom:4px;}";
    html += ".switch input{opacity:0;width:0;height:0;}";
    html += ".slider{position:absolute;cursor:pointer;top:0;left:0;right:0;bottom:0;background:#ccc;transition:.3s;border-radius:24px;}";
    html += ".slider:before{position:absolute;content:'';height:16px;width:16px;left:4px;bottom:4px;background:white;transition:.3s;border-radius:50%;}";
    html += "input:checked + .slider{background:#28a745;}";
    html += "input:checked + .slider:before{transform:translateX(21px);}";
    html += "</style></head><body>";

    html += "<div class='card'><h2>⚙️ Pannello Configurazione</h2>";

    html += "<div class='sezione'>";

    if(!relayRunning())
        html += "<form action='/start' method='POST'><button type='submit' class='btn-start'>▶️ Avvia Ciclo Manuale</button></form>";
    else
        html += "<form action='/stop' method='POST'><button type='submit' class='btn-stop'>🚨 Stop Emergenza</button></form>";

    html += "</div>";

    html += "<div class='sezione'><h3>⏱️ Durata Irrigazione Valvole</h3>";
    html += "<form action='/salva-durata' method='POST'>";
    html += "<input type='number' name='durata' min='1' value='" + String(config.relayDuration) + "' required>";
    html += "<button type='submit' class='btn-save'>Salva Durata</button>";
    html += "</form></div>";

    if(rtcGetStatus().available)
    {
        DateTime dt = rtcNow();

        char oraAttuale[6];
        snprintf(oraAttuale,sizeof(oraAttuale),"%02d:%02d",dt.hour(),dt.minute());

        html += "<div class='sezione'><h3>📅 Aggiorna Ora Orologio</h3>";
        html += "<form action='/salva-ora' method='POST'>";
        html += "<input type='time' name='orario' value='" + String(oraAttuale) + "' required>";
        html += "<button type='submit' class='btn-save'>Salva Ora</button>";
        html += "</form></div>";
    }

    html += "<div class='sezione'><h3>🔌 Maschera Abilitazione Valvole</h3>";
    html += "<form action='/salva-maschere' method='POST'>";

    html += "<div class='grid-toggle'>";
    html += generaHtmlRele();
    html += "</div>";

    html += "<h3>📆 Giorni Programmazione Active</h3>";

    html += "<div class='grid-days'>";
    html += generaHtmlGiorni();
    html += "</div>";

    html += "<button type='submit' class='btn-save'>Salva Interruttori</button>";
    html += "</form></div>";

    html += "<div class='sezione'>";
    html += "<a href='/'><button class='btn-back'>Torna al Monitoraggio</button></a>";
    html += "</div>";

    html += "</div></body></html>";

    server.send(200,"text/html",html);
}

#endif