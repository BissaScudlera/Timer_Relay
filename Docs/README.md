# Timer_Relay

##Librerie utilizzate
RTClib      https://github.com/adafruit/RTClib  
            testato con: v2.1.4
Dipende da: Adafruit BusIO https://github.com/adafruit/Adafruit_BusIO   
            testato con: v1.17.4

MCP23017    https://github.com/blemasle/arduino-mcp23017
            testato con: v2.0.0 


## Descrizione
Firmware ESP32 per gestione irrigazione con:
- gestione relè/valvole;
- programmazione oraria;
- RTC;
- interfaccia Web;
- diagnostica sistema;
- configurazione WiFi AP.

## Stato architettura
Il progetto è organizzato in moduli:
- RelayManager: gestione uscite;
- Scheduler: logica temporale;
- RTCManager: data/ora e temperatura RTC;
- ConfigManager: parametri irrigazione;
- WebServer/WebPages/WebHandlers: interfaccia web;
- Diagnostics: stato sistema.

## Avvio
All'accensione:
1. inizializzazione hardware;
2. inizializzazione RTC;
3. avvio WiFi AP;
4. avvio Web Server;
5. gestione ciclo irrigazione.
