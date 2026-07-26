1---------
non salva config in memoria non volatile (Preferences, EEPROM, LittleFS, ecc.).
implementare funzioni tipo:

configSave();
configLoad();

Vanno chiamate al termine di gestisciSalvaMaschere()

2---------
all'interfaccia web 

nella pagina comando:
In futuro qui si può aggiungere anche:
Avanza valvola
Test relè (debug)

nella pagina sistema:
sincronizzazione RTC;
riavvio ESP32;
reset configurazione;
informazioni di diagnostica;
uptime;
memoria libera;
temperatura ESP32.
