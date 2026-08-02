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


## Interfaccia
HTTP SERVER Defaults: IP:192.168.4.1, porta:80

Digital Output:
        -Error: si è verificato un errore dall'ultimo riavvio (attualmente LED_BUILTIN)
        -AP_ON: il server è attivo(To Do)

Digital Input: 
        -pulsante start
                  pressione rapida: avvio manuale ciclo irrigazione

        -pulsante stop
                  pressione rapida: arresta il ciclo di irrigazione corrente
                  pressione prolungata(5s): -commuta l'accensione dell'AP wifi 
                                            -funzione da abilitare tramite jumper?

jumper di configuazione:
        -reset impostazioni default (quando attivo al riavvio le impostazioni NVM vengono sovrascritte)
        -debug setup (quando attivo il debug seriale, attende connessione prima di iniziare il Loop()
        -abilitazione AP (To Do: forza stato accensione AP)

flag compilatore:
        -DEBUG: abilita comunicazione seriale e messaggi stato/errore
        -MCP1_ENABLE, MCP2_ENABLE: abilita le interfacce I/O I2C
        -RELAY_NUMBER: numero di uscite relè assegnateal ciclo automatico

impostazioni default:
        -wifi_config.h       impostazioni AP e webserver(To Do)
        -ConfigManager.cpp   impostazioni timer rele
        -Timer_Relay.ino     impostazioni indirizzi I/O,