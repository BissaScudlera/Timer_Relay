# RAM e ottimizzazioni

## Attenzione

La gestione dinamica delle stringhe WiFi deve evitare allocazioni ripetute.

Possibile miglioramento:
- sostituire puntatori char dinamici con String globali o buffer statici;
- centralizzare gestione configurazioni.

## Web

Le pagine HTML costruite con String sono accettabili su ESP32, ma possono essere ottimizzate se la UI cresce.
