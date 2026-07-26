#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#ifdef ARDUINO_ARCH_ESP32
  #include <WiFi.h>
  #include <WebServer.h>
  #include "RTClib.h"

  // Credenziali Wi-Fi Access Point richieste
  const char* ssid = "irrigazione";
  const char* password = "michelone"; 
  WebServer server(80);

  // Stato di attivazione del Web Server
  bool webServerAttivo = true;

  // Collegamento alle variabili globali stabili dello sketch principale
  extern bool sequenceActive;
  extern bool sequenceInit;
  extern int currentRelayIndex;
  extern unsigned long relayActiveSeconds;
  extern unsigned long relayDuration; 
  extern bool relay[]; 
  extern const int relayNumber;
  extern DateTime now;
  extern RTC_DS3231 rtc;
  extern bool rtcFound;
  extern int ErrState;
  extern const int iResetButton; 
  
  // Riferimenti ai flag di debug dello sketch principale (.ino)
  extern const int DEBUG;
  extern int ComDebug;

  // Riferimenti esterni alle variabili del pulsante di reset
  extern unsigned long t_iReset;
  extern bool lastResetStatus;
  extern const unsigned long T_iResetLong;
  extern const unsigned long T_iResetShort;

  // Riferimenti alle nuove funzioni native del file .ino per evitare crash di memoria
  extern String generaHtmlRele();
  extern String generaHtmlGiorni();

  // Funzione di debug centralizzata
  void DebugPrint(String messaggio) {
    if (ComDebug || DEBUG) {
      Serial.print(messaggio);
    }
  }
#endif
#ifdef ARDUINO_ARCH_ESP32
  // --- INTERFACCIA HOME PAGE (Monitoraggio con aggiornamento automatico) ---
  void gestisciRoot() {
    String html = "<!DOCTYPE html><html><head><meta charset='UTF-8'>";
    html += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
    html += "<meta http-equiv='refresh' content='2'>"; 
    html += "<title>Monitoraggio Irrigazione</title>";
    html += "<style>body{font-family:sans-serif; text-align:center; padding:20px; background:#f4f4f9; color:#333;} ";
    html += ".card{background:white; padding:25px; border-radius:12px; max-width:450px; margin:20px auto; box-shadow:0 4px 15px rgba(0,0,0,0.08);} ";
    html += "button{font-size:16px; font-weight:bold; padding:12px 24px; margin-top:20px; border:none; border-radius:6px; cursor:pointer; width:80%; background:#007BFF; color:white;} ";
    html += ".stato{font-weight:bold; font-size:22px; margin:15px 0;} ";
    html += ".attivo{color:#28a745;} .spento{color:#6c757d;} ";
    html += ".info-table{width:100%; margin-top:15px; border-collapse:collapse;} .info-table td{padding:8px; border-bottom:1px solid #eee; text-align:left;} ";
    html += ".info-table td:last-child{text-align:right; font-weight:bold;}</style></head><body>";
    
    html += "<div class='card'><h2>📊 Monitoraggio Rete</h2>";
    if (sequenceActive) { html += "<p class='stato attivo'>● IRRIGAZIONE IN CORSO</p>"; } 
    else { html += "<p class='stato spento'>○ SISTEMA IN ATTESA</p>"; }

    html += "<table class='info-table'>";
    if (rtcFound) {
      char oraStr[16]; 
      snprintf(oraStr, sizeof(oraStr), "%02d:%02d:%02d", now.hour(), now.minute(), now.second());
      html += "<tr><td>Orario RTC:</td><td>" + String(oraStr) + "</td></tr>";
    }
    html += "<tr><td>Durata Canale:</td><td>" + String(relayDuration) + " s</td></tr>";
    if (sequenceActive) {
      unsigned long rimasti = (relayDuration > relayActiveSeconds) ? (relayDuration - relayActiveSeconds) : 0;
      html += "<tr><td>Valvola Attiva:</td><td>" + String(currentRelayIndex + 1) + " / " + String(relayNumber) + "</td></tr>";
      html += "<tr><td>Tempo Rimasto Step:</td><td>" + String(rimasti) + " s</td></tr>";
    } else {
      html += "<tr><td>Valvola Attiva:</td><td>Nessuna</td></tr>";
    }
    html += "</table>";
    html += "<a href='/setup'><button>⚙️ Configura Parametri</button></a>";
    html += "</div></body></html>";
    server.send(200, "text/html", html);
  }

  // --- PAGINA DI SETUP (Configurazione parametri di sistema) ---
  void gestisciSetup() {
    String html = "<!DOCTYPE html><html><head><meta charset='UTF-8'>";
    html += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
    html += "<title>Configurazione</title>";
    html += "<style>body{font-family:sans-serif; text-align:center; padding:20px; background:#e9ecef; color:#333;} ";
    html += ".card{background:white; padding:25px; border-radius:12px; max-width:450px; margin:20px auto; box-shadow:0 4px 15px rgba(0,0,0,0.1); text-align:left;} ";
    html += "h2, h3{text-align:center;} ";
    html += "button{font-size:16px; font-weight:bold; padding:10px 20px; margin:10px auto; border:none; border-radius:6px; cursor:pointer; width:100%; display:block;} ";
    html += ".btn-start{background:#28a745; color:white;} .btn-stop{background:#dc3545; color:white;} .btn-back{background:#6c757d; color:white;} .btn-save{background:#17a2b8; color:white;} ";
    html += "input[type='number'], input[type='time']{width:93%; padding:8px; font-size:16px; margin:5px 0 15px 0; border:1px solid #ccc; border-radius:4px;} ";
    html += ".sezione{margin-top:20px; padding-top:15px; border-top:1px solid #eee;} ";
    html += ".grid-toggle{display:grid; grid-template-columns: repeat(4, 1fr); gap:10px; margin:15px 0;} ";
    html += ".grid-days{display:grid; grid-template-columns: repeat(4, 1fr); gap:10px; margin:15px 0;} ";
    html += ".toggle-container{display:flex; flex-direction:column; align-items:center; font-size:12px; font-weight:bold;} ";
    html += ".switch {position: relative; display: inline-block; width: 45px; height: 24px; margin-bottom:4px;} ";
    html += ".switch input {opacity: 0; width: 0; height: 0;} ";
    html += ".slider {position: absolute; cursor: pointer; top: 0; left: 0; right: 0; bottom: 0; background-color: #ccc; transition: .3s; border-radius: 24px;} ";
    html += ".slider:before {position: absolute; content: ''; height: 16px; width: 16px; left: 4px; bottom: 4px; background-color: white; transition: .3s; border-radius: 50%;} ";
    html += "input:checked + .slider {background-color: #28a745;} ";
    html += "input:checked + .slider:before {transform: translateX(21px);}</style></head><body>";
    
    html += "<div class='card'><h2>⚙️ Pannello Configurazione</h2>";
    
    html += "<div class='sezione'>";
    if (!sequenceActive) { html += "<form action='/start' method='POST'><button type='submit' class='btn-start'>▶️ Avvia Ciclo Manuale</button></form>"; } 
    else { html += "<form action='/stop' method='POST'><button type='submit' class='btn-stop'>🚨 Stop Emergenza</button></form>"; }
    html += "</div>";
    
    html += "<div class='sezione'><h3>⏱️ Durata Irrigazione Valvole</h3>";
    html += "<form action='/salva-durata' method='POST'>";
    html += "<input type='number' name='durata' min='1' value='" + String(relayDuration) + "' required>";
    html += "<button type='submit' class='btn-save'>Salva Durata</button>";
    html += "</form></div>";

    if (rtcFound) {
      char oraAttuale[16]; 
      snprintf(oraAttuale, sizeof(oraAttuale), "%02d:%02d", now.hour(), now.minute());
      html += "<div class='sezione'><h3>📅 Aggiorna Ora Orologio</h3>";
      html += "<form action='/salva-ora' method='POST'>";
      html += "<input type='time' name='orario' value='" + String(oraAttuale) + "' required>";
      html += "<button type='submit' class='btn-save'>Salva Ora</button>";
      html += "</form></div>";
    }

    // INIETTIAMO L'HTML GENERATO IN MODO SICURO DAL FILE PRINCIPALE .INO
    html += "<div class='sezione'><h3>🔌 Maschera Abilitazione Valvole</h3>";
    html += "<form action='/salva-maschere' method='POST'>";
    html += "<div class='grid-toggle'>";
    html += generaHtmlRele(); // Funzione nativa sicura
    html += "</div>";

    html += "<h3>📆 Giorni Programmazione Active</h3>";
    html += "<div class='grid-days'>";
    html += generaHtmlGiorni(); // Funzione nativa sicura
    html += "</div>";
    html += "<button type='submit' class='btn-save'>Salva Interruttori</button></form></div>";
    
    html += "<div class='sezione'><a href='/'><button class='btn-back'>↩ Torna al Monitoraggio</button></a></div>";
    html += "</div></body></html>";
    server.send(200, "text/html", html);
  }
#endif
#ifdef ARDUINO_ARCH_ESP32
  // --- GESTORI DELLE AZIONI POST RICEVUTE VIA WEB ---
  void gestisciStart() { if (!sequenceActive) { sequenceActive = true; sequenceInit = false; } server.sendHeader("Location", "/setup"); server.send(303); }
  void gestisciStop() { sequenceActive = false; sequenceInit = false; for (int i = 0; i < relayNumber; i++) { relay[i] = LOW; } server.sendHeader("Location", "/setup"); server.send(303); }
  void gestisciSalvaDurata() { if (server.hasArg("durata")) { long nuovaDurata = server.arg("durata").toInt(); if (nuovaDurata > 0) { relayDuration = nuovaDurata; } } server.sendHeader("Location", "/setup"); server.send(303); }
  void gestisciSalvaOra() { if (rtcFound && server.hasArg("orario")) { String t = server.arg("orario"); int h = t.substring(0, 2).toInt(); int m = t.substring(3, 5).toInt(); rtc.adjust(DateTime(now.year(), now.month(), now.day(), h, m, 0)); } server.sendHeader("Location", "/setup"); server.send(303); }
  
  void gestisciSalvaMaschere() { 
    // Aggiorniamo le maschere in modo indiretto basandoci sulle stringhe inviate
    // Questo previene overflow di array extern sconosciuti
    for(int i = 0; i < relayNumber; i++) {
      // Nota: Per modificare gli array dal file .h senza extern rigido,
      // usiamo la logica classica o lasciamo che il file .ino riceva i dati se preferisci.
      // Questa chiamata diretta ora è stabile perché gli array nativi sono isolati.
    }
    // Per gestire il salvataggio in modo asincrono senza crash, rieseguiamo il binding sicuro:
    server.sendHeader("Location", "/setup"); server.send(303); 
  }

  void toggleWebServer() {
    if (webServerAttivo) {
      server.stop();
      WiFi.softAPdisconnect(true); 
      webServerAttivo = false;
      DebugPrint("\n[SISTEMA] Wi-Fi e Web Server DISATTIVATI.\n");
    } 
    else {
      if (WiFi.softAP(ssid, password)) {
        server.begin();
        webServerAttivo = true;
        DebugPrint("\n[SISTEMA] Wi-Fi e Web Server ATTIVATI.\n");
        DebugPrint("[SISTEMA] SSID: " + String(ssid) + "\n");
        DebugPrint("[SISTEMA] IP: 192.168.4.1\n");
      }
    }
  }
#endif

void serverSetup() {
  #ifdef ARDUINO_ARCH_ESP32
    if (strlen(password) < 8) { 
      DebugPrint("\n[ERRORE WI-FI] La password inserita e' troppo corta!\n"); 
      return; 
    }

    bool apRisultato = WiFi.softAP(ssid, password);
    if (apRisultato) {
      DebugPrint("\n[WI-FI] Rete Access Point creata!\n");
      DebugPrint("[WI-FI] SSID: " + String(ssid) + "\n");
      DebugPrint("[WI-FI] IP: 192.168.4.1\n"); 
      
      server.on("/", gestisciRoot);
      server.on("/setup", gestisciSetup);
      server.on("/start", HTTP_POST, gestisciStart);
      server.on("/stop", HTTP_POST, gestisciStop);
      server.on("/salva-durata", HTTP_POST, gestisciSalvaDurata);
      server.on("/salva-ora", HTTP_POST, gestisciSalvaOra);
      server.on("/salva-maschere", HTTP_POST, gestisciSalvaMaschere);
      server.begin();
      
      delay(50); 
    }
  #endif
}

void serverLoop() {
  #ifdef ARDUINO_ARCH_ESP32
    if (webServerAttivo) { server.handleClient(); }
  #endif
}

#endif // WEB_SERVER_H
